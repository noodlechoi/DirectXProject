#pragma once

// 포인터 ComPtr로 바꾸기
// d3d device를 다루고 Scene을 관리
class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE, HWND);
	void OnDestroy();

	// 스왑 체인, 디바이스, 서술자 힙, 명령 큐/ 할당자/ 리스트 생성 함수
	void CreateSwapChain();
	void CreateRtvAndDsvHeaps();
	void CreateD3DDevice();
	void CreateCommandQueueAndList();

	// 렌더 타겟 뷰, 깊이-스텐실 뷰 생성 함수
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	// 렌더링할 메쉬와 게임 객체 생성 및 소멸 함수
	void BuildObjects();
	void ReleaseObjects();

	// 사용자 입력, 애니메이션, 렌더링 함수
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	// 윈도우 메시지 처리 함수
	void waitForGpuComplete();

	//CPU, GPU 동기화 함수
	void OnProcessMouseMessage(HWND , UINT , WPARAM , LPARAM );
	void OnProcessKeyboardMessage(HWND , UINT , WPARAM , LPARAM );
	LRESULT CALLBACK OnProcessWindowMessage(HWND, UINT, WPARAM, LPARAM);
private:
	HINSTANCE h_instance;
	HWND h_wnd;

	int client_width{FRAME_BUFFER_WIDTH};
	int client_height{FRAME_BUFFER_HEIGHT};

	IDXGIFactory4* dxgi_factory{};
	IDXGISwapChain3* swap_chain{};
	ID3D12Device* d3d_device{};

	// MSAA 다중 샘플링 활성화 및 다중 샘플링 레벨 설정
	bool msaa4x_enabled{};
	UINT msaa4x_quality_level{};

	static const UINT swap_chain_buffer_num{2};
	UINT swap_chain_buffer_index{};

	// 렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소 크기
	ID3D12Resource* render_target_buffers[swap_chain_buffer_num]{};
	ID3D12DescriptorHeap* rtv_descriptor_heap{};
	UINT rtv_increment_size{};

	// 깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소 크기
	ID3D12Resource* depth_stencil_buffer{};
	ID3D12DescriptorHeap* dsv_descriptor_heap{};
	UINT dsv_increment_size{};

	// 명령 큐, 명형 할당자, 명령 리스트 인터페이스 포인터
	ID3D12CommandQueue* command_queue{};
	ID3D12CommandAllocator* command_allocator{};
	ID3D12GraphicsCommandList* command_list{};

	// 그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터
	ID3D12PipelineState* pipeline_state{};

	// 펜스 인터페이스 포인터, 펜스 값, 이벤트 핸들
	ID3D12Fence* fence{};
	UINT64 fence_value{};
	HANDLE fence_event{};

	// 뷰포트와 씨저 사각형
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissor_rect{};
};

