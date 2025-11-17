#pragma once
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "InputManager.h"

class CScene
{
public:
	CScene() = default;
	~CScene() = default;

	void ReleaseUploadBuffers();

	virtual void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*);

	void AnimateObjects(float);
	virtual void ProcessInput() {};

	// 멤버 변수 set
	void Render(ID3D12GraphicsCommandList*);
protected:
	std::vector<std::unique_ptr<CShader>> shaders{};
	std::deque<std::unique_ptr<CObject>> objects{};
	std::shared_ptr<CCamera> camera;
};

class CTitleScene : public CScene {
public:
	CTitleScene() = default;
	~CTitleScene() = default;

	void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*) override;
	void ProcessInput() override;

private:
	std::unique_ptr<InputManager> input_manager;
	
	RECT start_button{ 100, 150, 680, 240 };
	RECT exit_button{240, 250, 550, 330};
};

class CGameScene : public CScene {
public:
	void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*) override;
	void ProcessInput() override;
private:
	POINT old_cursor_pos;
};