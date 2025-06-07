#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	_tcscpy_s(frame_rate_str, _T("LapProject ("));
}

CGameFramework::~CGameFramework()
{

}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	h_instance = hInstance;
	h_wnd = hMainWnd;

	// Direct3D ����̽�, ��� ť�� ��� ����Ʈ ����, ���� ü�� �� ���� �Լ� ȣ��
	try {
		CreateD3DDevice();
		CreateCommandQueueAndList();
		CreateRtvAndDsvHeaps();
		CreateSwapChain();
		CreateDepthStencilView();
	}
	catch (...)
	{
		OutputDebugString(L"Oncreate �Լ� ����\n");
	}

	// ������ ���� ��ü ����
	BuildObjects();

	return true;
}
void CGameFramework::OnDestroy()
{
	waitForGpuComplete();

	ReleaseObjects();

	::CloseHandle(fence_event);

	swap_chain->SetFullscreenState(FALSE, nullptr);
#if defined(_DEBUG)
	// ���ҽ� ���� Ȯ��
	IDXGIDebug1* dxgi_debug = nullptr;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&dxgi_debug);
	HRESULT result = dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	dxgi_debug->Release();
#endif
}

// ���� ü��, ����̽�, ������ ��, ��� ť/ �Ҵ���/ ����Ʈ ���� �Լ�
void CGameFramework::CreateSwapChain()
{
	RECT rc;
	GetClientRect(h_wnd, &rc);
	client_width = rc.right - rc.left;
	client_height = rc.bottom - rc.top;

	// �����ϱ�05
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = swap_chain_buffer_num;
	swapChainDesc.BufferDesc.Width = client_width;
	swapChainDesc.BufferDesc.Height = client_height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = h_wnd;
	swapChainDesc.SampleDesc.Count = (msaa4x_enabled) ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = (msaa4x_enabled) ? (msaa4x_quality_level - 1) : 0;
	swapChainDesc.Windowed = TRUE;
	// ��ü ȭ�� ��忡�� ����ȭ���� �ػ󵵸� �ĸ� ������ ũ�⿡ �°� ����
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(dxgi_factory->CreateSwapChain(command_queue.Get(), &swapChainDesc, (IDXGISwapChain**)swap_chain.GetAddressOf()));
	swap_chain_buffer_index = swap_chain->GetCurrentBackBufferIndex();
	ThrowIfFailed(dxgi_factory->MakeWindowAssociation(h_wnd, DXGI_MWA_NO_ALT_ENTER));	// alt+enter�� �������� �ʰ� ����
#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE 
	CreateRenderTargetViews();
#endif // 

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
	// factory ����
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(IDXGIFactory4), (void**)&dxgi_factory));

	// �����(�׷���ī��)
	IDXGIAdapter1* adapter{};
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != dxgi_factory->EnumAdapters1(i, &adapter); ++i) {
		DXGI_ADAPTER_DESC1 adapterDesc;
		adapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;	// ������ ����̹����?
		if(SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&d3d_device))) break;	// direct12 ����̽� ����
	}
	if (!adapter) {	// 12�� �����ϴ� ����̽��� ������ �� ������ WARP ����̽� ����
		dxgi_factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&adapter);
		D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&d3d_device);
	}

	// ����̽��� �����ϴ� ���� ���� ǰ�� ���� Ȯ��
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaAQualityLevels;
	msaaAQualityLevels.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	msaaAQualityLevels.SampleCount = 4;	// Msaa4x ���� ���ø�
	msaaAQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msaaAQualityLevels.NumQualityLevels = 0;
	d3d_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaAQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	msaa4x_quality_level = msaaAQualityLevels.NumQualityLevels;

	// ǰ�� ������ 1���� ũ�� ���� ���ø� Ȱ��ȭ
	msaa4x_enabled = (msaa4x_quality_level > 1) ? true : false;

	// ����ȭ�� ���� fence ����
	result = d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fence);
	for(int i = 0; i < swap_chain_buffer_num; ++i)
		fence_value[i] = 0;

	// �潺�� ����ȭ�� ���� �̺�Ʈ ��ü ����. signal �� �̺�Ʈ ���� �ڵ������� FALSE�� �ǵ��� ����
	/*
	HANDLE CreateEvent(
	LPSECURITY_ATTRIBUTES lpEventAttributes, // ���� �Ӽ�
	BOOL bManualReset,                      // ����/�ڵ� ���� ����
	BOOL bInitialState,                     // �ʱ� ���� (��ȣ/���ȣ)
	LPCSTR lpName                           // �̺�Ʈ ��ü �̸�
	);
	*/
	fence_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	if (adapter) adapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	// GPU ����� ���� �ɸ� �� time out ���� �� ���� ���� ����
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	// GPU�� ��� ��� ���� ����

	// ���� ��� ť ����
	ThrowIfFailed(d3d_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&command_queue));

	// �Ҵ��� ����
	ThrowIfFailed(d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&command_allocator));

	//����Ʈ ����
	ThrowIfFailed(d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator.Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)command_list.GetAddressOf()));

	// ����Ʈ�� �����Ǹ� Open���°� ��
	ThrowIfFailed(command_list->Close());
}


void CGameFramework::CreateRtvAndDsvHeaps()
{
	// ���� Ÿ�� ������ ��(������ ���� = ����ü�� ������ ����) ����
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = swap_chain_buffer_num;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;

	ThrowIfFailed(d3d_device->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&rtv_descriptor_heap));

	// ���� Ÿ�� ������ �� ������ ũ�� ����
	rtv_increment_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// ����-���Ľ� ������ ��(1��) ����
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ThrowIfFailed(d3d_device->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&dsv_descriptor_heap));

	// ���� ũ�� ����
	dsv_increment_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateRenderTargetViews()
{
	// ����ü���� �� �ĸ� ���ۿ� ���� ���� Ÿ�� �� ����
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle = rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < swap_chain_buffer_num; ++i) {
		ThrowIfFailed(swap_chain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&render_target_buffers[i]));
		d3d_device->CreateRenderTargetView(render_target_buffers[i].Get(), NULL, rtvCPUDescriptorHandle);
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
	// ���� ����
	ThrowIfFailed(d3d_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, __uuidof(ID3D12Resource), (void**)&depth_stencil_buffer));

	// �� ����
	// �� ���� �ڵ� ��
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDesctiptorHandle = dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	d3d_device->CreateDepthStencilView(depth_stencil_buffer.Get(), NULL, dsvCPUDesctiptorHandle);
}

// �������� �޽��� ���� ��ü ���� �� �Ҹ� �Լ�
void CGameFramework::BuildObjects()
{
	command_list->Reset(command_allocator.Get(), NULL);
	// ī�޶� ��ü ����
	camera = std::make_unique<CCamera>();
	camera->SetViewport(0, 0, client_width, client_height);
	camera->SetScissorRect(0, 0, client_width, client_height);
	camera->GenerateProjectionMatrix(1.0f, 500.0f, (float)client_width / (float)client_height, 90.0f);
	camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	// �� ��ü ����
	now_scene = std::make_unique<CScene>();
	if (now_scene) now_scene->BuildObjects(d3d_device.Get(), command_list.Get());

	// �׷��� ��� ����Ʈ ��� ť�� �߰�
	command_list->Close();
	ID3D12CommandList* commandLists[] = { command_list.Get()};
	command_queue->ExecuteCommandLists(1, commandLists);

	//�׷��� ��� ����Ʈ���� ��� ����� ������ ��ٸ���.
	waitForGpuComplete();

	//�׷��� ���ҽ����� �����ϴ� ������ ������ ���ε� ���۵��� �Ҹ��Ų��.
	if (now_scene) now_scene->ReleaseUploadBuffers();

	timer.Reset();
}

void CGameFramework::ReleaseObjects()
{
}

// ����� �Է�, �ִϸ��̼�, ������ �Լ�
void CGameFramework::ProcessInput()
{

}

void CGameFramework::AnimateObjects()
{
	if (now_scene) now_scene->AnimateObjects(timer.GetTimeElapsed());
}

void CGameFramework::waitForGpuComplete()
{
	// CPU �潺 �� ����
	const UINT64 fenceValue = ++fence_value[swap_chain_buffer_index];
	// GPU�� �潺 ���� �����ϴ� ����� ��ɾ� ť�� �߰�
	ThrowIfFailed(command_queue->Signal(fence.Get(), fenceValue));
	// GPU �潺 ���� CPU �潺 ������ ������ ��� �̺�Ʈ�� ��ٸ�
	if (fence->GetCompletedValue() < fenceValue) {
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fence_event));
		::WaitForSingleObject(fence_event, INFINITE);
	}
}

void CGameFramework::ChangeSwapChainState()
{
	waitForGpuComplete();

	BOOL fullScreenState{};
	swap_chain->GetFullscreenState(&fullScreenState, NULL);
	swap_chain->SetFullscreenState(!fullScreenState, NULL);	// full -> window, window -> full

	DXGI_MODE_DESC targetParameters;
	targetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	targetParameters.Width = client_width;
	targetParameters.Height = client_height;
	targetParameters.RefreshRate.Numerator = 60;
	targetParameters.RefreshRate.Denominator = 1;
	targetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	targetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain->ResizeTarget(&targetParameters);

	// ResizeBuffers�� �����ϱ� ���� ��� ��/���� ������ �����ؾ� ��
	for (int i = 0; i < swap_chain_buffer_num; ++i) {
		if (render_target_buffers[i]) render_target_buffers[i].Reset();
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swap_chain->GetDesc(&swapChainDesc);
	swap_chain->ResizeBuffers(swap_chain_buffer_num, client_width, client_height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);

	swap_chain_buffer_index = swap_chain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void CGameFramework::MoveToNextFrame()
{
	swap_chain_buffer_index = swap_chain->GetCurrentBackBufferIndex();

	UINT64 fenceValue = ++fence_value[swap_chain_buffer_index];
	ThrowIfFailed(command_queue->Signal(fence.Get(), fenceValue));
	if (fence->GetCompletedValue() < fenceValue) {
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fence_event));
		WaitForSingleObject(fence_event, INFINITE);
	}
}


void CGameFramework::FrameAdvance()
{
	timer.Tick(0.0f);

	ProcessInput();

	AnimateObjects();

	// ��� ����
	ThrowIfFailed(command_allocator->Reset());
	ThrowIfFailed(command_list->Reset(command_allocator.Get(), NULL));

	// ���� ���� Ÿ�ٿ� ���� ������Ʈ�� �����⸦ ��ٸ�. ������Ʈ�� ������ ���� Ÿ�� ���·� �ٲ�
	D3D12_RESOURCE_BARRIER resourceBarrier{};
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// render target state�� ���ҽ� ����
	resourceBarrier.Transition.pResource = render_target_buffers[swap_chain_buffer_index].Get();
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	command_list->ResourceBarrier(1, &resourceBarrier);

	// ���� ���� Ÿ�ٿ� �ش��ϴ� �������� CPU �ּ�(�ڵ�) ���� ���
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDesciptorHandle = rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	rtvCPUDesciptorHandle.ptr += (swap_chain_buffer_index * rtv_increment_size);

	// ����-���ٽ� �������� CPU �ּҸ� ����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDescriptorHandle = dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();

	// ���� Ÿ�� ��� ���� ���ٽ� �並 ��� ���� �ܰ�(OM)�� ����
	command_list->OMSetRenderTargets(1, &rtvCPUDesciptorHandle, FALSE, &dsvCPUDescriptorHandle);

	// ���ϴ� �������� ���� Ÿ�� �����
	float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	command_list->ClearRenderTargetView(rtvCPUDesciptorHandle, clearColor, 0, NULL);

	// ���ϴ� ������ ���� ���ٽ� �����
	command_list->ClearDepthStencilView(dsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0F, 0, 0, NULL);

	// ������ �ڵ�
	if (now_scene) now_scene->Render(command_list.Get(), camera.get());

	// ���� ���� Ÿ�ٿ� ���� �������� �����⸦ ��ٸ�. GPU�� ���۸� �� �̻� ������� ������ ���� Ÿ�� -> ������Ʈ ���·� ����
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	command_list->ResourceBarrier(1, &resourceBarrier);

	ThrowIfFailed(command_list->Close());

	ID3D12CommandList* commandLists[]{ command_list.Get() };
	command_queue->ExecuteCommandLists(1, commandLists);

	waitForGpuComplete();

	// ����ü�� ������Ʈ. ���� ���� Ÿ���� ������ ���� ���۷� �Ű����� ���� Ÿ�� �ε����� �ٲ�
	swap_chain->Present(0, 0);

	MoveToNextFrame();

	timer.GetFrameRate(frame_rate_str + 12, 37);
	::SetWindowText(h_wnd, frame_rate_str);
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
			 ChangeSwapChainState();
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

