#include "D3D12Resources.h"
#include "D3D12Core.h"

namespace primal::graphics::d3d12
{
	bool descriptor_heap::initialize(u32 capacity, bool is_shader_visible)
	{
		std::lock_guard lock{ mutex_ };
		assert(capacity && capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2);
		assert(!(type_ == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER &&
				 capacity > D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE));
		if (type_ == D3D12_DESCRIPTOR_HEAP_TYPE_DSV ||
			type_ == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		{
			is_shader_visible = false;
		}

		release();

		ID3D12Device* const device{ core::device() };
		assert(device);

		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Flags = is_shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NumDescriptors = capacity;
		desc.Type = type_;
		desc.NodeMask = 0;

		
		HRESULT hr = DXCall(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap_)));
		if (FAILED(hr))
		{
			
			return false;
		}

		free_handles_ = std::move(std::make_unique<u32[]>(capacity));
		capacity_ = capacity;
		size_ = 0;
		for (u32 i = 0; i < capacity; ++i)
		{
			free_handles_[i] = i;
		}
		DEBUG_OP(for (u32 i = 0; i < frame_buffer_count; ++i) assert(deferred_free_indices_[i].empty()));

		descriptor_size_ = device->GetDescriptorHandleIncrementSize(type_);
		cpu_start_ = heap_->GetCPUDescriptorHandleForHeapStart();
		gpu_start_ = is_shader_visible ? heap_->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{0};


		return true;
	}

	void descriptor_heap::release()
	{
		assert(!size_);
		core::defered_release(heap_);
	}

	void descriptor_heap::process_deferred_free(u32 frame_index)
	{
		std::lock_guard lock{ mutex_ };
		assert(frame_index < frame_buffer_count);
		
		utl::vector<u32>& indices{ deferred_free_indices_[frame_index] };
		if (not indices.empty())
		{
			for (auto index : indices)
			{
				--size_;
				free_handles_[size_] = index;
			}
			indices.clear();
		}
	}

	descriptor_handle descriptor_heap::allocate()
	{
		std::lock_guard lock{ mutex_ };
		assert(heap_);
		assert(size_ < capacity_);

		const u32 index{ free_handles_[size_] };
		const u32 offset{ index * descriptor_size_ };
		++size_;

		descriptor_handle handle{};
		handle.cpu.ptr = cpu_start_.ptr + offset;
		if (is_shader_visible())
		{
			handle.gpu.ptr = gpu_start_.ptr + offset;
		}
		DEBUG_OP(handle.container_ = this);
		DEBUG_OP(handle.index_ = index);
		return handle;
	}

	void descriptor_heap::free(descriptor_handle handle)
	{
		if (not handle.is_valid())
		{
			return;
		}
		std::lock_guard lock{ mutex_ };
		assert(heap_ && size_);
		assert(handle.container_ == this);
		assert(handle.cpu.ptr >= cpu_start_.ptr);
		assert((handle.cpu.ptr - cpu_start_.ptr) % descriptor_size_ == 0);
		assert(handle.index_ < capacity_);
		const u32 index{ (u32)(handle.cpu.ptr - cpu_start_.ptr) / descriptor_size_ };
		assert(index == handle.index_);

		const u32 frame_index{ core::current_frame_index() };
		deferred_free_indices_[frame_index].push_back(index);
		core::set_deferred_release_flag();
		handle = {};
	}
}
