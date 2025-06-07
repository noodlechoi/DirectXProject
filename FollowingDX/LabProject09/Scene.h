#pragma once
#include "Camera.h"
#include "Object.h"

// 게임 오브젝트를 관리하는 클래스
class CScene
{
public:
	CScene();
	virtual ~CScene();

	ID3D12RootSignature* GetGraphicsRootSignature() { return graphics_root_signature.Get(); }

	bool OnProcessMouseMessage(HWND, UINT, WPARAM, LPARAM);
	bool OnProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device*);
	void ReleaseUploadBuffers();

	void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*);

	bool ProcessInput();
	void AnimateObjects(float);

	// 멤버 변수 set
	void PrepareRender(ID3D12GraphicsCommandList*);
	void Render(ID3D12GraphicsCommandList*, CCamera*);
protected:
	std::deque<std::unique_ptr<CGameObject>> objects{};

	ComPtr<ID3D12RootSignature> graphics_root_signature{};
};

