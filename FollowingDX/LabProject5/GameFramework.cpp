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
	if (rtv_heap)
		rtv_heap->Release();
	if (dsv_heap)
		dsv_heap->Release();
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

void CGameFramework::CreateRtvAndDsvHeaps()
{

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
	if (!adapter) {
		//dxgi_factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1))
	}
}

void CGameFramework::CreateCommandQueueAndList()
{

}

// ���� Ÿ�� ��, ����-���ٽ� �� ���� �Լ�
void CGameFramework::CreateRenderTargetViews()
{

}
void CGameFramework::CreateDepthStencilView()
{

}

// �������� �޽��� ���� ��ü ���� �� �Ҹ� �Լ�
void BuildObjects();
void ReleaseObjects();

// ����� �Է�, �ִϸ��̼�, ������ �Լ�
void ProcessInput();
void AnimateObjects();
void FrameAdvance();

// ������ �޽��� ó�� �Լ�
void waitForGpuComplete();

//CPU, GPU ����ȭ �Լ�
void OnProcessMouseMessage(HWND, UINT, WPARAM, LPARAM);
void OnProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK OnProcessWindowMessage(HWND, UINT, WPARAM, LPARAM);