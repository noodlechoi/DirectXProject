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

	// Direct3D ����̽�, ��� ť�� ��� ����Ʈ ����, ���� ü�� �� ���� �Լ� ȣ��
	CreateD3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	// ������ ���� ��ü ����
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

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc{};
	//::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = client_width;
	dxgiSwapChainDesc.Height = client_height;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = msaa4x_enabled ? 4 : 1;		// MSAA Ȱ��ȭ ���ο� ���� ���ø� �� ����
	dxgiSwapChainDesc.SampleDesc.Quality = msaa4x_enabled ? (msaa4x_quality_level - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// �ĸ� ���ۿ� ���� ǥ�� ��� ��İ� CPU ���� ���: ���� Ÿ������ ���
	dxgiSwapChainDesc.BufferCount = swap_chain_buffer_num;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;						// �ػ� Ȯ�� �����ϸ�?
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// ���� ó�� ���: ������
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;			// �⺻
	dxgiSwapChainDesc.Flags = 0;										// ����ü�� ���� ����

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiFullScreenDesc{};
	//::ZeroMemory(&dxgiFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST;	// ���� �ʵ�� �̹��� ����
	dxgiFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiFullScreenDesc.Windowed = TRUE;

	// ����ü�� ����
	dxgi_factory->CreateSwapChainForHwnd(command_queue, h_wnd, &dxgiSwapChainDesc, &dxgiFullScreenDesc, nullptr, (IDXGISwapChain1**)&swap_chain);

	// alt+enter Ű ��Ȱ��ȭ
	dxgi_factory->MakeWindowAssociation(h_wnd, DXGI_MWA_NO_ALT_ENTER);
	// ���� ü�� ���� �ĸ� ���� �ε��� ����
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
	// factory ����
	result = ::CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(IDXGIFactory4), (void**)dxgi_factory);

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
		D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)d3d_device);
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
	result = d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)fence);
	fence_value = 0;

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

	// ����Ʈ�� ������ Ŭ���̾�Ʈ ��ü �������� ����
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(client_width);
	viewport.Height = static_cast<float>(client_height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// ���� �簢���� Ŭ���̾�Ʈ ���� ��ü�� ����
	scissor_rect = { 0, 0, client_width, client_height };

	if (adapter) adapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	// GPU ����� ���� �ɸ� �� time out ���� �� ���� ���� ����
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	// GPU�� ��� ��� ���� ����

	// ���� ��� ť ����
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)command_queue));

	// �Ҵ��� ����
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&command_allocator));

	//����Ʈ ����
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&command_list));

	// ����Ʈ�� �����Ǹ� Open���°� ��
	CHECK_HRESULT_EXCEPTION(command_list->Close());
}


void CGameFramework::CreateRtvAndDsvHeaps()
{
	// ���� Ÿ�� ������ ��(������ ���� = ����ü�� ������ ����) ����
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = swap_chain_buffer_num;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;

	CHECK_HRESULT_EXCEPTION(d3d_device->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)rtv_descriptor_heap));

	// ���� Ÿ�� ������ �� ������ ũ�� ����
	rtv_increment_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// ����-���Ľ� ������ ��(1��) ����
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)dsv_descriptor_heap));

	// ���� ũ�� ����
	dsv_increment_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateRenderTargetViews()
{
	// ����ü���� �� �ĸ� ���ۿ� ���� ���� Ÿ�� �� ����
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
	// ���� ����
	CHECK_HRESULT_EXCEPTION(d3d_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, __uuidof(ID3D12Resource), (void**)&depth_stencil_buffer));

	// �� ����
	// �� ���� �ڵ� ��
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDesctiptorHandle = dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	d3d_device->CreateDepthStencilView(depth_stencil_buffer, NULL, dsvCPUDesctiptorHandle);
}

// �������� �޽��� ���� ��ü ���� �� �Ҹ� �Լ�
void CGameFramework::BuildObjects()
{

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

}

void CGameFramework::FrameAdvance()
{
	ProcessInput();
	
	AnimateObjects();

	// ��� ����
	CHECK_HRESULT_EXCEPTION(command_allocator->Reset());
	CHECK_HRESULT_EXCEPTION(command_list->Reset(command_allocator, NULL));

	// ���� ���� Ÿ�ٿ� ���� ������Ʈ�� �����⸦ ��ٸ�. ������Ʈ�� ������ ���� Ÿ�� ���·� �ٲ�
	D3D12_RESOURCE_BARRIER resourceBarrier{};
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// render target state�� ���ҽ� ����
	resourceBarrier.Transition.pResource = render_target_buffers[swap_chain_buffer_index];
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	command_list->ResourceBarrier(1, &resourceBarrier);
	
	// ����Ʈ ���� �簢�� ����
	command_list->RSSetViewports(1, &viewport);
	command_list->RSSetScissorRects(1, &scissor_rect);

	// ���� ���� Ÿ�ٿ� �ش��ϴ� �������� CPU �ּ�(�ڵ�) ���� ���
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDesciptorHandle = rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	rtvCPUDesciptorHandle.ptr += (swap_chain_buffer_index * rtv_increment_size);

	// ���ϴ� �������� ���� Ÿ�� �����
	//float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };	// == Colors::Azure
	command_list->ClearRenderTargetView(rtvCPUDesciptorHandle, Colors::Azure, 0, NULL);
	
	// ����-���ٽ� �������� CPU �ּҸ� ����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDescriptorHandle = dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	
	// ���ϴ� ������ ���� ���ٽ� �����
	command_list->ClearDepthStencilView(dsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0F, 0, 0, NULL);
	
	// ���� Ÿ�� ��� ���� ���ٽ� �並 ��� ���� �ܰ�(OM)�� ����
	command_list->OMSetRenderTargets(1, &rtvCPUDesciptorHandle, TRUE, &dsvCPUDescriptorHandle);

	// ������ �ڵ�

	// ���� ���� Ÿ�ٿ� ���� �������� �����⸦ ��ٸ�. GPU�� ���۸� �� �̻� ������� ������ ���� Ÿ�� -> ������Ʈ ���·� ����
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
	// ����ü�� ������Ʈ. ���� ���� Ÿ���� ������ ���� ���۷� �Ű����� ���� Ÿ�� �ε����� �ٲ�

	swap_chain_buffer_index = swap_chain->GetCurrentBackBufferIndex();
	swap_chain->Present1(1, 0, &dxgiPresentParameters);
}

void CGameFramework::waitForGpuComplete()
{
	// CPU �潺 �� ����
	++fence_value;

	const UINT64 fenceValue = fence_value;
	// GPU�� �潺 ���� �����ϴ� ����� ��ɾ� ť�� �߰�
	CHECK_HRESULT_EXCEPTION(command_queue->Signal(fence, fenceValue));
	// GPU �潺 ���� CPU �潺 ������ ������ ��� �̺�Ʈ�� ��ٸ�
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