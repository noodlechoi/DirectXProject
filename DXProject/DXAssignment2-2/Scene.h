#pragma once
#include "Shader.h"
#include "Object.h"

class CScene
{
public:
	CScene();
	~CScene();

	void ReleaseUploadBuffers();

	void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*);
	void ReleaseObjects();

	void AnimateObjects(float);

	// 멤버 변수 set
	void Render(ID3D12GraphicsCommandList*);
protected:
	std::vector<std::unique_ptr<CShader>> shaders{};
	std::deque<std::unique_ptr<CObject>> objects{};

};

