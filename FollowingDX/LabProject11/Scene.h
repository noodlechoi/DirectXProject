#pragma once
#include "Player.h"
#include "Object.h"

// ���� ������Ʈ�� �����ϴ� Ŭ����
class CScene
{
public:
	CScene(float, float);
	virtual ~CScene();

	ID3D12RootSignature* GetGraphicsRootSignature() { return graphics_root_signature.Get(); }

	bool OnProcessMouseMessage(HWND, UINT, WPARAM, LPARAM);
	bool OnProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device*);
	void ReleaseUploadBuffers();

	void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*);

	bool ProcessInput();
	void AnimateObjects(float);

	// ��� ���� set
	void PrepareRender(ID3D12GraphicsCommandList*);
	void Render(ID3D12GraphicsCommandList*);
protected:
	ComPtr<ID3D12RootSignature> graphics_root_signature{};

	std::unique_ptr<CPlayer> player{};
	std::vector<CObjectShader> shaders{};
};

