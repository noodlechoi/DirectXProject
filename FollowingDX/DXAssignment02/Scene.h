#pragma once
#include "Shader.h"
#include "InputManager.h"

class CScene
{
public:
	CScene() {}
	CScene(CRollerCoasterInputManager*, float, float);
	virtual ~CScene();

	ID3D12RootSignature* GetGraphicsRootSignature() { return graphics_root_signature.Get(); }

	bool OnProcessMouseMessage(HWND, UINT, WPARAM, LPARAM);
	bool OnProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device*);
	void ReleaseUploadBuffers();

	virtual void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*);

	virtual void ProcessInput(HWND&, float);
	virtual void AnimateObjects(float);

	// ¸â¹ö º¯¼ö set 
	void PrepareRender(ID3D12GraphicsCommandList*);
	void Render(ID3D12GraphicsCommandList*);

	virtual CScene* NextScene() { return new CScene; };
protected:
	ComPtr<ID3D12RootSignature> graphics_root_signature{};

	std::vector<CObjectShader> shaders{};
	std::unique_ptr<CRollerCoasterInputManager> input_manager;

	float cliend_width{};
	float cliend_height{};
};


class CRollerCoasterScene : public CScene {
public:
	CRollerCoasterScene(float, float);

	void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*) override;
	CScene* NextScene() override;
};

class CTankScene : public CScene {
public:
	CTankScene(float, float);

	void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*) override;
	void CheckObjectByBulletCollisions();
	void AnimateObjects(float) override;
};