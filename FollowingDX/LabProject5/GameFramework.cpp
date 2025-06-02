#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
}

CGameFramework::~CGameFramework()
{

}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	h_instance = hInstance;
	h_wnd = hMainWnd;

	// Direct3D 디바이스, 명령 큐와 명령 리스트 생성, 스왑 체인 등 생성 함수 호출
	CreateD3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	// 렌더링 게임 객체 생성
	BuildObjects();

	return true;
}
void CGameFramework::OnDestroy()
{
	waitForGpuComplete();

	ReleaseObjects();

	::CloseHandle(fence_event);

	for (int i = 0; i < swap_chain_buffer_num; ++i)
		if (render_target_buffers[i])
			render_target_buffers[i]->Release();
	if (rtv_descriptor_heap)
		rtv_descriptor_heap->Release();
	if (dsv_descriptor_heap)
		dsv_descriptor_heap->Release();
	if (depth_stencil_buffer)
		depth_stencil_buffer->Release();

	if (command_allocator)
		command_allocator->Release();
	if (command_queue)
		command_queue->Release();
	if (pipeline_state)
		pipeline_state->Release();
	if (command_list)
		command_list->Release();

	if(fence)
		fence->Release();

	swap_chain->SetFullscreenState(FALSE, nullptr);
	if (swap_chain)
		swap_chain->Release();
	if (dxgi_factory)
		dxgi_factory->Release();
	if (d3d_device)
		d3d_device->Release();
#if defined(_DEBUG)
	// 리소스 누수 확인
	IDXGIDebug1* dxgi_debug = nullptr;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&dxgi_debug);
	HRESULT result = dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	dxgi_debug->Release();
#endif
}

// 스왑 체인, 디바이스, 서술자 힙, 명령 큐/ 할당자/ 리스트 생성 함수
void CGameFramework::CreateSwapChain()
{
	RECT rc;
	GetClientRect(h_wnd, &rc);
	client_width = rc.right - rc.left;
	client_height = rc.bottom - rc.top;

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc{};
	//::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = client_width;
	dxgiSwapChainDesc.Height = client_height;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = msaa4x_enabled ? 4 : 1;		// MSAA 활성화 여부에 따라 샘플링 수 설정
	dxgiSwapChainDesc.SampleDesc.Quality = msaa4x_enabled ? (msaa4x_quality_level - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 후면 버퍼에 대한 표면 사용 방식과 CPU 접근 방법: 렌더 타겟으로 사용
	dxgiSwapChainDesc.BufferCount = swap_chain_buffer_num;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;						// 해상도 확대 스케일링?
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// 스왑 처리 방식: 버리기
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;			// 기본
	dxgiSwapChainDesc.Flags = 0;										// 스왑체인 동작 설정

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiFullScreenDesc{};
	//::ZeroMemory(&dxgiFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST;	// 하위 필드로 이미지 생성
	dxgiFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiFullScreenDesc.Windowed = TRUE;

	// 스왑체인 생성
	dxgi_factory->CreateSwapChainForHwnd(command_queue, h_wnd, &dxgiSwapChainDesc, &dxgiFullScreenDesc, nullptr, (IDXGISwapChain1**)&swap_chain);

	// alt+enter 키 비활성화
	dxgi_factory->MakeWindowAssociation(h_wnd, DXGI_MWA_NO_ALT_ENTER);
	// 스왑 체인 현재 후면 버퍼 인덱스 저장
	swap_chain_buffer_index = swap_chain->GetCurrentBackBufferIndex();
}



void CGameFramework::CreateD3DDevice()
{
	HRESULT result;
	UINT dxgiFactoryFlags{};
#if defined(_DEBUG)
	ID3D12Debug* d3dDebugController{};
	result = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&d3dDebugController);
	if (d3dDebugController) {
		d3dDebugController->EnableDebugLayer();
		d3dDebugController->Release();
	}
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	// factory 생성
	result = ::CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(IDXGIFactory4), (void**)dxgi_factory);

	// 어댑터(그래픽카드)
	IDXGIAdapter1* adapter{};
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != dxgi_factory->EnumAdapters1(i, &adapter); ++i) {
		DXGI_ADAPTER_DESC1 adapterDesc;
		adapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;	// 렌더링 드라이버라면?
		if(SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&d3d_device))) break;	// direct12 디바이스 생성
	}
	if (!adapter) {	// 12를 지원하는 디바이스를 생성할 수 없으면 WARP 디바이스 생성
		dxgi_factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&adapter);
		D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)d3d_device);
	}

	// 디바이스가 지원하는 다중 샘플 품질 수준 확인
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaAQualityLevels;
	msaaAQualityLevels.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	msaaAQualityLevels.SampleCount = 4;	// Msaa4x 다중 샘플링
	msaaAQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msaaAQualityLevels.NumQualityLevels = 0;
	d3d_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaAQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	msaa4x_quality_level = msaaAQualityLevels.NumQualityLevels;

	// 품질 수준이 1보다 크면 다중 샘플링 활성화
	msaa4x_enabled = (msaa4x_quality_level > 1) ? true : false;

	// 동기화를 위한 fence 생성
	result = d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)fence);
	fence_value = 0;

	// 펜스와 동기화를 위한 이벤트 객체 생성. signal 시 이벤트 값을 자동적으로 FALSE가 되도록 생성
	/*
	HANDLE CreateEvent(
	LPSECURITY_ATTRIBUTES lpEventAttributes, // 보안 속성
	BOOL bManualReset,                      // 수동/자동 리셋 여부
	BOOL bInitialState,                     // 초기 상태 (신호/비신호)
	LPCSTR lpName                           // 이벤트 객체 이름
	);
	*/
	fence_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	// 뷰포트를 윈도우 클라이언트 전체 영역으로 설정
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(client_width);
	viewport.Height = static_cast<float>(client_height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// 씨저 사각형을 클라이언트 영역 전체로 설정
	scissor_rect = { 0, 0, client_width, client_height };

	if (adapter) adapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	// GPU 계산이 오래 걸릴 때 time out 실행 후 다음 명형 실행
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	// GPU가 모든 명령 직접 실행

	// 직접 명령 큐 생성
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)command_queue));

	// 할당자 생성
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&command_allocator));

	//리스트 생성
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&command_list));

	// 리스트가 생성되면 Open상태가 됨
	CHECK_HRESULT_EXCEPTION(command_list->Close());
}


void CGameFramework::CreateRtvAndDsvHeaps()
{
	// 렌더 타겟 서술자 힙(서술자 개수 = 스왑체인 버퍼의 개수) 생성
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = swap_chain_buffer_num;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;

	CHECK_HRESULT_EXCEPTION(d3d_device->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)rtv_descriptor_heap));

	// 렌더 타겟 서술자 힙 원소의 크기 저장
	rtv_increment_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 깊이-스탠실 서술자 힙(1개) 생성
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)dsv_descriptor_heap));

	// 원소 크기 저장
	dsv_increment_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateRenderTargetViews()
{
	// 스왑체인의 각 후면 버퍼에 대한 렌더 타겟 뷰 생성
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle = rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < swap_chain_buffer_num; ++i) {
		CHECK_HRESULT_EXCEPTION(swap_chain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&render_target_buffers[i]));
		d3d_device->CreateRenderTargetView(render_target_buffers[i], NULL, rtvCPUDescriptorHandle);
		rtvCPUDescriptorHandle.ptr += rtv_increment_size;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = client_width;
	resourceDesc.Height = client_height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = (msaa4x_enabled) ? 4 : 1;
	resourceDesc.SampleDesc.Quality = (msaa4x_enabled) ? (msaa4x_quality_level - 1) : 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	// 버퍼 생성
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, __uuidof(ID3D12Resource), (void**)&depth_stencil_buffer));

	// 뷰 생성
	// 힙 시작 핸들 값
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDesctiptorHandle = dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	d3d_device->CreateDepthStencilView(depth_stencil_buffer, NULL, dsvCPUDesctiptorHandle);
}

// 렌더링할 메쉬와 게임 객체 생성 및 소멸 함수
void CGameFramework::BuildObjects()
{

}

void CGameFramework::ReleaseObjects()
{

}

// 사용자 입력, 애니메이션, 렌더링 함수
void CGameFramework::ProcessInput()
{

}

void CGameFramework::AnimateObjects()
{

}

void CGameFramework::FrameAdvance()
{
	ProcessInput();
	
	AnimateObjects();

	// 명령 리셋
	CHECK_HRESULT_EXCEPTION(command_allocator->Reset());
	CHECK_HRESULT_EXCEPTION(command_list->Reset(command_allocator, NULL));

	// 현재 렌더 타겟에 대한 프리젠트가 끝나기를 기다림. 프리젠트가 끝나면 렌더 타겟 상태로 바꿈
	D3D12_RESOURCE_BARRIER resourceBarrier{};
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// render target state로 리소스 변경
	resourceBarrier.Transition.pResource = render_target_buffers[swap_chain_buffer_index];
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	command_list->ResourceBarrier(1, &resourceBarrier);
	
	// 뷰포트 씨저 사각형 설정
	command_list->RSSetViewports(1, &viewport);
	command_list->RSSetScissorRects(1, &scissor_rect);

	// 현재 렌더 타겟에 해당하는 서술자의 CPU 주소(핸들) 값을 계산
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDesciptorHandle = rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	rtvCPUDesciptorHandle.ptr += (swap_chain_buffer_index * rtv_increment_size);

	// 원하는 색상으로 렌더 타겟 지우기
	//float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };	// == Colors::Azure
	command_list->ClearRenderTargetView(rtvCPUDesciptorHandle, Colors::Azure, 0, NULL);
	
	// 깊이-스텐실 서술자의 CPU 주소를 계산한다.
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDescriptorHandle = dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	
	// 원하는 값으로 깊이 스텐실 지우기
	command_list->ClearDepthStencilView(dsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0F, 0, 0, NULL);
	
	// 렌더 타겟 뷰와 깊이 스텐실 뷰를 출력 병합 단계(OM)에 연결
	command_list->OMSetRenderTargets(1, &rtvCPUDesciptorHandle, TRUE, &dsvCPUDescriptorHandle);

	// 렌더링 코드

	// 현재 렌더 타겟에 대한 렌더링이 끝나기를 기다림. GPU가 버퍼를 더 이상 사용하지 않으면 렌더 타겟 -> 프레젠트 상태로 변경
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	command_list->ResourceBarrier(1, &resourceBarrier);
	
	CHECK_HRESULT_EXCEPTION(command_list->Close());

	ID3D12CommandList* commandLists[]{ command_list };
	command_queue->ExecuteCommandLists(1, commandLists);

	waitForGpuComplete();

	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	// 스왑체인 프리젠트. 현재 렌더 타겟의 내용이 전면 버퍼로 옮겨지고 렌더 타겟 인덱스가 바뀜

	swap_chain_buffer_index = swap_chain->GetCurrentBackBufferIndex();
	swap_chain->Present1(1, 0, &dxgiPresentParameters);
}

void CGameFramework::waitForGpuComplete()
{
	// CPU 펜스 값 증가
	++fence_value;

	const UINT64 fenceValue = fence_value;
	// GPU가 펜스 값을 설정하는 명령을 명령어 큐에 추가
	CHECK_HRESULT_EXCEPTION(command_queue->Signal(fence, fenceValue));
	// GPU 펜스 값이 CPU 펜스 값보다 작으면 계속 이벤트를 기다림
	if (fence->GetCompletedValue() < fenceValue) {
		CHECK_HRESULT_EXCEPTION(fence->SetEventOnCompletion(fenceValue, fence_event));
		::WaitForSingleObject(fence_event, INFINITE);
	}
}

void CGameFramework::OnProcessMouseMessage(HWND hWnd, UINT MessageID, WPARAM wParam, LPARAM lParam)
{
	switch (MessageID) {
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessKeyboardMessage(HWND hWnd, UINT MessageID, WPARAM wParam, LPARAM lParam)
{
	switch (MessageID) {
	case WM_KEYUP:
		 switch (wParam) {
		 case VK_ESCAPE:
			 ::PostQuitMessage(0);
			 break;
		 case VK_RETURN:
			 break;
		 case VK_F8:
			 break;
		 case VK_F9:
			 break;
		 default:
			 break;
		 }
		 break;
	default:
		 break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessWindowMessage(HWND hWnd, UINT MessageID, WPARAM wParam, LPARAM lParam)
{
	switch (MessageID) {
	case WM_SIZE:
	{
		client_width = LOWORD(lParam);
		client_height = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessMouseMessage(hWnd, MessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessKeyboardMessage(hWnd, MessageID, wParam, lParam);
		break;
	}
	return 0;
}