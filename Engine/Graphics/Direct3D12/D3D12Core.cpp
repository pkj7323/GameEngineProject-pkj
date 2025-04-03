#include "D3D12Core.h"
#include "D3D12Resources.h"
#include "D3D12Surface.h"

using namespace Microsoft::WRL;
namespace primal::graphics::d3d12::core
{
	namespace 
	{
		class d3d12_command
		{
		public:
			d3d12_command() = default;
			DISABLE_COPY_AND_MOVE(d3d12_command);
			explicit d3d12_command(ID3D12Device8* const device, D3D12_COMMAND_LIST_TYPE type)
			{
				HRESULT hr{ S_OK };
				D3D12_COMMAND_QUEUE_DESC desc{};
				desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				desc.NodeMask = 0;
				desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				desc.Type = type;
				DXCall(hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue_)));
				if (FAILED(hr))
				{
					release();
					return;
				}
				NAME_D3D12_OBJECT(command_queue_,
								  type == D3D12_COMMAND_LIST_TYPE_DIRECT ? 
								  L"GFX Command Queue" :
								  type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
								  L"Compute Command Queue" : L"Command Queue");
				for (size_t i = 0; i < command_frames_.size(); ++i)
				{
					command_frame& frame = command_frames_[i];
					// 이제 frame의 인덱스는 i입니다.
					DXCall(hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&frame.command_allocator_)));
					if (FAILED(hr))
					{
						release();
						return;
					}
					NAME_D3D12_OBJECT_INDEXED(frame.command_allocator_, i,
											  type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
											  L"GFX Command Allocator" :
											  type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
											  L"Compute Command Allocator" : L"Command Allocator");
				}



				DXCall(hr = device->CreateCommandList(0, type, command_frames_[0].command_allocator_,
													  nullptr, IID_PPV_ARGS(&command_list_)));
				if (FAILED(hr))
				{
					release();
					return;
				}
				DXCall(command_list_->Close());
				NAME_D3D12_OBJECT(command_list_,
								  type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
								  L"GFX Command List" :
								  type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
								  L"Compute Command List" : L"Command List");

				hr = DXCall(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
				if (FAILED(hr))
				{
					release();
					return;
				}
				NAME_D3D12_OBJECT(fence_, L"Command Queue Fence");

				fence_event_ = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
				assert(fence_event_);


				return;
			}
			~d3d12_command()
			{
				assert(!command_queue_ && !command_list_ && !fence_);
			}



			void flush()
			{
				for (auto& frame : command_frames_)
				{
					frame.wait(fence_event_, fence_);
				}
				frame_index_ = 0;
			}

			void release()
			{
				flush();
				core::release(fence_);
				fence_value_ = 0;
				
				CloseHandle(fence_event_);
				fence_event_ = nullptr;
				

				core::release(command_list_);
				core::release(command_queue_);
				for (auto& frame : command_frames_)
				{
					frame.release();
				}
			}

			void begin_frame()
			{
				command_frame& frame{ command_frames_[frame_index_] };
				frame.wait(fence_event_,fence_);
				DXCall(frame.command_allocator_->Reset());
				DXCall(command_list_->Reset(frame.command_allocator_, nullptr));
			}

			void end_frame()
			{
				DXCall(command_list_->Close());
				ID3D12CommandList* const command_lists[]{ command_list_ };
				command_queue_->ExecuteCommandLists(_countof(command_lists), &command_lists[0]);

				u64& fence_value{ fence_value_ };
				++fence_value;
				command_frame& frame{ command_frames_[frame_index_] };
				frame.fence_value_ = fence_value;
				command_queue_->Signal(fence_, fence_value_);

				frame_index_ = (frame_index_ + 1) % frame_buffer_count;
			}

			constexpr ID3D12CommandQueue* get_command_queue() const { return command_queue_; }
			constexpr ID3D12GraphicsCommandList6* get_command_list() const { return command_list_; }
			constexpr u32 get_frame_index() const { return frame_index_; }
		private:
			struct command_frame
			{
				ID3D12CommandAllocator* command_allocator_{ nullptr };
				u64						fence_value_{ 0 };
				void wait(HANDLE fence_event, ID3D12Fence1* fence)
				{
					// 현재 펜스 값이 여전히 "fence_value"보다 작으면
					// GPU가 명령 목록 실행을 완료하지 않은 것을 알 수 있습니다.
					// "_cmd_queue->Signal()" 명령에 도달하지 않았기 때문입니다.
					assert(fence && fence_event);
					if (fence->GetCompletedValue() < fence_value_)
					{
						// 펜스에서 이벤트를 생성하여 펜스의 현재 값이 "fence_value"와 같으면 신호를 보냅니다.
						DXCall(fence->SetEventOnCompletion(fence_value_, fence_event));
						// 펜스가 이벤트를 트리거하여 현재 값이 "fence_value"에 도달할 때까지 기다립니다.
						// 명령 큐가 실행을 완료했음을 나타냅니다.
						WaitForSingleObject(fence_event, INFINITE);
					}
					
				}
				void release()
				{
					core::release(command_allocator_);
					fence_value_ = 0;
				}
			};
			ID3D12CommandQueue*								command_queue_{ nullptr };
			ID3D12GraphicsCommandList6*						command_list_{ nullptr };

			ID3D12Fence1*									fence_{ nullptr };
			u64												fence_value_{ 0 };

			std::array<command_frame, frame_buffer_count>	command_frames_{}; //문제 발생하면 c스타일로 stl쓰지말고 내려야됨

			HANDLE											fence_event_{ nullptr };
			u32												frame_index_{0};
		};

		ID3D12Device8*				main_device{ nullptr };
		IDXGIFactory7*				dxgi_factory{ nullptr };
		d3d12_command				gfx_command;
		descriptor_heap				rtv_desc_heap{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV };
		descriptor_heap				dsv_desc_heap{ D3D12_DESCRIPTOR_HEAP_TYPE_DSV };
		descriptor_heap				srv_desc_heap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV };
		descriptor_heap				uav_desc_heap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV };
		constexpr D3D_FEATURE_LEVEL minimum_feature_level{ D3D_FEATURE_LEVEL_11_0 };

		u32							deferred_release_flag[frame_buffer_count]{};
		std::mutex					deferred_release_mutex{};
		utl::vector<IUnknown*>		deferred_releases[frame_buffer_count]{};




		bool failed_init()
		{
			shutdown();
			return false;
		}
		IDXGIAdapter4* determine_main_adapter()
		{
			IDXGIAdapter4* adapter{ nullptr };
			//가장　좋은　그래픽　카드를　찾는다．
			for (u32 i{}; dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
																   IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				if (SUCCEEDED(D3D12CreateDevice(adapter, minimum_feature_level, __uuidof(ID3D12Device), nullptr)))
				{
					return adapter;
				}
				release(adapter);
			}

			return nullptr; // 유능한 그래픽 카드를 찾지 못함
		}
		D3D_FEATURE_LEVEL get_max_feature_level(IDXGIAdapter4* adapter)
		{
			constexpr std::array<D3D_FEATURE_LEVEL, 4> feature_levels
			{
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_12_1,
			};

			D3D12_FEATURE_DATA_FEATURE_LEVELS feature_levels_info{};
			feature_levels_info.NumFeatureLevels = feature_levels.size();
			feature_levels_info.pFeatureLevelsRequested = feature_levels.data();

			ComPtr<ID3D12Device> device;
			DXCall(D3D12CreateDevice(adapter, minimum_feature_level, IID_PPV_ARGS(&device)));
			DXCall(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &feature_levels_info, sizeof(feature_levels_info)));
			return feature_levels_info.MaxSupportedFeatureLevel;
		}

		void __declspec(noinline) process_deferred_releases(u32 frame_index)
		{
			std::lock_guard lock{ deferred_release_mutex };
			deferred_release_flag[frame_index] = 0;

			rtv_desc_heap.process_deferred_free(frame_index);
			dsv_desc_heap.process_deferred_free(frame_index);
			srv_desc_heap.process_deferred_free(frame_index);
			uav_desc_heap.process_deferred_free(frame_index);

			utl::vector<IUnknown*>& resources{ deferred_releases[frame_index] };
			if (!resources.empty())
			{
				for (auto& resource : resources) release(resource);
				resources.clear();
			}
		}
	}

	namespace detail
	{
		void deferred_release(IUnknown* resource)
		{
			const u32 frame_index{ current_frame_index() };
			std::lock_guard lock{ deferred_release_mutex };
			deferred_releases[frame_index].push_back(resource);
			set_deferred_release_flag();
		}
	}

	bool initialize()
	{
		// 무슨 어뎁터(예를 들어 그래픽카드 같은)를 쓰는지 알아내야함
		// 그 어뎁터가 제공 할 수 있는 가장 최대 최신 버전이 무엇인지 알아내야함
		// ID3D12Device를 생성해야함(이것은 가상 어뎁터임)
		if (main_device)
		{
			shutdown();
		}
		u32 dxgi_factory_flags{ 0 };
#ifdef _DEBUG
		//디버그 레이어 활성화 하려면 컴퓨터에서 설정->선택적 기능 추가->그래픽 도구 가 추가되어있어야함
		{
			ComPtr<ID3D12Debug3> debug_interface;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface))))
			{
				debug_interface->EnableDebugLayer();
			}
			else
			{
				OutputDebugStringA("디버그 레이어 활성화 하려면 컴퓨터에서 설정->선택적 기능 추가->그래픽 도구 가 추가되어있어야함\n");
			}
			dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		HRESULT hr{ S_OK };
		//CreateDXGIFactory2(0, __uuidof(IDXGIFactory7), (void**)dxgi_factory); 의 매크로
		hr = CreateDXGIFactory2(0,IID_PPV_ARGS(&dxgi_factory));
		if (FAILED(hr))
		{
			return failed_init();
		}

		// 무슨 어뎁터(예를 들어 그래픽카드 같은)를 쓰는지 알아내야함
		ComPtr<IDXGIAdapter4> main_adapter;
		main_adapter.Attach(determine_main_adapter());
		if (not main_adapter)
		{
			return failed_init();
		}

		D3D_FEATURE_LEVEL max_feature_level{ get_max_feature_level(main_adapter.Get()) };
		assert(max_feature_level >= minimum_feature_level);
		if (max_feature_level < minimum_feature_level)
		{
			return failed_init();
		}

		DXCall(hr = D3D12CreateDevice(main_adapter.Get(), max_feature_level, IID_PPV_ARGS(&main_device)));
		if (FAILED(hr))
		{
			return failed_init();
		}
		bool result{ true };
		result &= rtv_desc_heap.initialize(512, false);
		result &= dsv_desc_heap.initialize(512, false);
		result &= srv_desc_heap.initialize(4096, true);
		result &= uav_desc_heap.initialize(512, true);
		if (not result)
		{
			return failed_init();
		}

		new (&gfx_command) d3d12_command(main_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		if (!gfx_command.get_command_queue())
		{
			return failed_init();
		}

		NAME_D3D12_OBJECT(main_device, L"Main Device");
		NAME_D3D12_OBJECT(rtv_desc_heap.heap(), L"RTV Descriptor Heap");
		NAME_D3D12_OBJECT(dsv_desc_heap.heap(), L"DSV Descriptor Heap");
		NAME_D3D12_OBJECT(srv_desc_heap.heap(), L"SRV Descriptor Heap");
		NAME_D3D12_OBJECT(uav_desc_heap.heap(), L"UAV Descriptor Heap");


#ifdef _DEBUG
		{
			ComPtr<ID3D12InfoQueue> info_queue;
			DXCall(main_device->QueryInterface(IID_PPV_ARGS(&info_queue)));

			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true); //오류
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true); //경고
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true); //에러
			
		}
#endif
		return true;
	}


	void shutdown()
	{
		gfx_command.release();
		release(dxgi_factory);

		for (u32 i = 0; i < frame_buffer_count; ++i)
		{
			process_deferred_releases(i);
		}

		rtv_desc_heap.release();
		dsv_desc_heap.release();
		srv_desc_heap.release();
		uav_desc_heap.release();

		process_deferred_releases(0);

#ifdef _DEBUG
		{
			{
				ComPtr<ID3D12InfoQueue> info_queue;
				DXCall(main_device->QueryInterface(IID_PPV_ARGS(&info_queue)));

				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false); //오류
				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false); //경고
				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false); //에러
			}

			ComPtr<ID3D12DebugDevice2> debug_device;
			DXCall(main_device->QueryInterface(IID_PPV_ARGS(&debug_device)));
			release(main_device);
			DXCall(debug_device->ReportLiveDeviceObjects(
				D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
		}
#endif

		release(main_device);
	}


	ID3D12Device *const device() { return main_device; }

	descriptor_heap& rtv_heap() { return rtv_desc_heap; }

	descriptor_heap& dsv_heap() { return dsv_desc_heap; }

	descriptor_heap& srv_heap() { return srv_desc_heap; }

	descriptor_heap& uav_heap() { return uav_desc_heap; }

	DXGI_FORMAT default_render_target_format() { return render_target_format; }

	u32 current_frame_index() { return gfx_command.frame_index(); }

	void render()
	{
		gfx_command.begin_frame();
		ID3D12GraphicsCommandList6* command_list{ gfx_command.get_command_list() };
		const u32 frame_index{ current_frame_index() };
		if (deferred_release_flag[frame_index])
		{
			process_deferred_releases(frame_index);
		}
		gfx_command.end_frame();
	}

	ID3D12Device* const device()
	{
		return main_device;
	}

	u32 current_frame_index()
	{
		return gfx_command.get_frame_index();
	}


	void set_deferred_release_flag()
	{
		deferred_release_flag[current_frame_index()] = 1;
	}
}
