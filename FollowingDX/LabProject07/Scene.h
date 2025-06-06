#pragma once
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	ID3D12RootSignature* GetGraphicsRootSignature() { return graphics_root_signature.Get(); }

	bool OnProcessMouseMessage(HWND, UINT, WPARAM, LPARAM);
	bool OnProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device*);
	void ReleaseUploadBuffers();

	void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float);

	// ¸â¹ö º¯¼ö set
	void PrepareRender(ID3D12GraphicsCommandList*);
	void Render(ID3D12GraphicsCommandList*);
protected:
	std::deque<std::unique_ptr<CShader>> shaders{};

	ComPtr<ID3D12RootSignature> graphics_root_signature{};
};

