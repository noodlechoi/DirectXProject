#pragma once
#include "Timer.h"
#include "Scene.h"

// d3d device�� �ٷ�� Scene�� ����
class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE, HWND);
	void OnDestroy();

	// ���� ü��, ����̽�, ������ ��, ��� ť/ �Ҵ���/ ����Ʈ ���� �Լ�
	void CreateSwapChain();
	void CreateRtvAndDsvHeaps();
	void CreateD3DDevice();
	void CreateCommandQueueAndList();

	// ���� Ÿ�� ��, ����-���ٽ� �� ���� �Լ�
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	// �������� �޽��� ���� ��ü ���� �� �Ҹ� �Լ�
	void BuildObjects();
	void ReleaseObjects();

	// ����� �Է�, �ִϸ��̼�, ������ �Լ�
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	// ������ �޽��� ó�� �Լ�
	void waitForGpuComplete();

	// Set State
	void ChangeSwapChainState();

	void MoveToNextFrame();

	//CPU, GPU ����ȭ �Լ�
	void OnProcessMouseMessage(HWND , UINT , WPARAM , LPARAM );
	void OnProcessKeyboardMessage(HWND , UINT , WPARAM , LPARAM );
	LRESULT CALLBACK OnProcessWindowMessage(HWND, UINT, WPARAM, LPARAM);
private:
	// window api ����
	HINSTANCE h_instance;
	HWND h_wnd;

	int client_width{FRAME_BUFFER_WIDTH};
	int client_height{FRAME_BUFFER_HEIGHT};

	// directX ����
	ComPtr<IDXGIFactory4> dxgi_factory{};
	ComPtr<IDXGISwapChain3> swap_chain{};
	ComPtr<ID3D12Device> d3d_device{};

	// MSAA ���� ���ø� Ȱ��ȭ �� ���� ���ø� ���� ����
	bool msaa4x_enabled{};
	UINT msaa4x_quality_level{};

	static const UINT swap_chain_buffer_num{2};
	UINT swap_chain_buffer_index{};

	// ���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ���� ũ��
	ComPtr<ID3D12Resource> render_target_buffers[swap_chain_buffer_num]{};
	ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap{};
	UINT rtv_increment_size{};

	// ����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ���� ũ��
	ComPtr<ID3D12Resource> depth_stencil_buffer{};
	ComPtr<ID3D12DescriptorHeap> dsv_descriptor_heap{};
	UINT dsv_increment_size{};

	// ��� ť, ���� �Ҵ���, ��� ����Ʈ �������̽� ������
	ComPtr<ID3D12CommandQueue> command_queue{};
	ComPtr<ID3D12CommandAllocator> command_allocator{};
	ComPtr<ID3D12GraphicsCommandList> command_list{};

	// �׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� ������
	ComPtr<ID3D12PipelineState> pipeline_state{};

	// �潺 �������̽� ������, �潺 ��, �̺�Ʈ �ڵ�
	ComPtr<ID3D12Fence> fence{};
	// �ĸ� ���۸��� �潺�� ����
	UINT64 fence_value[swap_chain_buffer_num]{};
	HANDLE fence_event{};

	// Timer ����
	CTimer timer;
	_TCHAR frame_rate_str[50];

	// game objects
	std::unique_ptr<CScene> now_scene{};
	std::unique_ptr<CCamera> camera{};


};

