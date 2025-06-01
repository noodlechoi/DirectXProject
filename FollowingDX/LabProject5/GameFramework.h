#pragma once

// ������ ComPtr�� �ٲٱ�
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

	//CPU, GPU ����ȭ �Լ�
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

	// MSAA ���� ���ø� Ȱ��ȭ �� ���� ���ø� ���� ����
	bool msaa4x_enabled{};
	UINT msaa4x_quality_level{};

	static const UINT swap_chain_buffer_num{2};
	UINT swap_chain_buffer_index{};

	// ���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ���� ũ��
	ID3D12Resource* render_target_buffers[swap_chain_buffer_num]{};
	ID3D12DescriptorHeap* rtv_descriptor_heap{};
	UINT rtv_increment_size{};

	// ����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ���� ũ��
	ID3D12Resource* depth_stencil_buffer{};
	ID3D12DescriptorHeap* dsv_descriptor_heap{};
	UINT dsv_increment_size{};

	// ��� ť, ���� �Ҵ���, ��� ����Ʈ �������̽� ������
	ID3D12CommandQueue* command_queue{};
	ID3D12CommandAllocator* command_allocator{};
	ID3D12GraphicsCommandList* command_list{};

	// �׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� ������
	ID3D12PipelineState* pipeline_state{};

	// �潺 �������̽� ������, �潺 ��, �̺�Ʈ �ڵ�
	ID3D12Fence* fence{};
	UINT64 fence_value{};
	HANDLE fence_event{};

	// ����Ʈ�� ���� �簢��
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissor_rect{};
};

