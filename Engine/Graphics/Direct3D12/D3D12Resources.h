#pragma once


#include "D3D12CommonHeader.h"

namespace primal::graphics::d3d12
{
	class descriptor_heap;
	struct descriptor_handle
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpu{};
		D3D12_GPU_DESCRIPTOR_HANDLE gpu{};

		constexpr bool is_valid() const
		{
			return cpu.ptr != 0;
		}
		constexpr bool is_shader_visible() const
		{
			return gpu.ptr != 0;
		}
#ifdef _DEBUG
	private:
		friend class descriptor_heap;
		descriptor_heap*	container_{ nullptr };
		u32 				index_{ u32_invalid_id };
#endif
	};
	class descriptor_heap
	{
	public:
		explicit descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE type) : type_{ type }
		{
			
		}
		DISABLE_COPY_AND_MOVE(descriptor_heap);
		~descriptor_heap() { assert(!heap_); }

		bool initialize(u32 capacity, bool is_shader_visible);
		void release();
		void process_deferred_free(u32 frame_index);

		[[nodiscard]] descriptor_handle allocate();
		void free(descriptor_handle& handle);

		constexpr D3D12_DESCRIPTOR_HEAP_TYPE type() const{ return type_; }
		constexpr D3D12_GPU_DESCRIPTOR_HANDLE gpu_start() const{ return gpu_start_; }
		constexpr D3D12_CPU_DESCRIPTOR_HANDLE cpu_start() const { return cpu_start_; }
		constexpr ID3D12DescriptorHeap* const heap() const { return heap_; }
		constexpr u32 capacity() const { return capacity_; }
		constexpr u32 size() const { return size_; }
		constexpr u32 descriptor_size() const { return descriptor_size_; }
		constexpr bool is_shader_visible() const{ return gpu_start_.ptr != 0; }
	private:
		ID3D12DescriptorHeap*				heap_;
		D3D12_GPU_DESCRIPTOR_HANDLE			gpu_start_{};
		D3D12_CPU_DESCRIPTOR_HANDLE			cpu_start_{};
		std::unique_ptr<u32[]>				free_handles_{};
		utl::vector<u32>					deferred_free_indices_[frame_buffer_count]{};
		std::mutex							mutex_{};
		u32 								capacity_{ 0 };
		u32 								size_{ 0 };
		u32									descriptor_size_{ 0 };
		const D3D12_DESCRIPTOR_HEAP_TYPE	type_{};

	};
}