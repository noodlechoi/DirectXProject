#pragma once

class CScene
{
public:
	CScene();
	~CScene();

	bool OnProcessMouseMessage(HWND, UINT, WPARAM, LPARAM);
	bool OnProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);

	void CreateGraphicsRootSignature(ID3D12Device*);
	void CreateGraphicsPiplineState(ID3D12Device*);

	void BuildObjects(ID3D12Device*);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float);

	// ¸â¹ö º¯¼ö set
	void PrepareRender(ID3D12GraphicsCommandList*);
	void Render(ID3D12GraphicsCommandList*);

	ComPtr<ID3D12RootSignature> graphics_root_signature{};
	ComPtr<ID3D12PipelineState> pipeline_state{};
};

