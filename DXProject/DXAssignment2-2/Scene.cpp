#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "Scene.h"

extern HWND ghWnd;

CScene::CScene()
{
}

CScene::~CScene()
{
	ReleaseObjects();
}

void CScene::ReleaseUploadBuffers()
{
	for (const auto& object : objects) {
		object->ReleaseUploadBuffer();
	}
}

void CScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	// 카메라 객체 생성
	RECT client_rect;
	GetClientRect(ghWnd, &client_rect);
	float width{ float(client_rect.right - client_rect.left) };
	float height{ float(client_rect.bottom- client_rect.top)};

	camera = std::make_unique<CCamera>();
	camera->SetViewport(0, 0, width, height);
	camera->SetScissorRect(0, 0, width, height);
	camera->GenerateProjectionMatrix(1.0f, 500.0f, (float)width / (float)height, 90.0f);
	camera->GenerateViewMatrix(XMFLOAT3(0.0f, 2.0f, -2.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	for (int i = 0; i < 1; ++i) {
		objects.push_back(std::make_unique<CObject>());
	}
	//CTriangleMesh* mesh= new CTriangleMesh(device, commandList);
	CRectangleMesh* mesh= new CRectangleMesh(device, commandList);
	objects[0]->SetMesh(mesh);
	CTexture* tex = new CTexture(std::string("stone"));
	tex->CreateTextureResource(device, commandList, std::wstring(L"Stone01.dds"));
	objects[0]->SetTexture(tex);

	shaders.push_back(std::make_unique<CTextureShader>());
	shaders[0]->CreateShader(device);
	shaders[0]->CreateShaderVariables(device, objects[0].get());
}

void CScene::ReleaseObjects()
{

}

void CScene::AnimateObjects(float elapsedTime)
{
	for (const auto& object : objects) {
		object->Animate(elapsedTime);
	}
}


void CScene::Render(ID3D12GraphicsCommandList* commandList)
{
	for (const auto& shader : shaders) {
		shader->PreRender(commandList);	// root signature set
	}
	// camera set
	camera->SetViewportsAndScissorRects(commandList);
	camera->UpdateShaderVariables(commandList);
	// object set
	for (const auto& object : objects) {
		object->UpdateShaderVariables(commandList);
	}

	// setpipeline
	for (const auto& shader : shaders) {
		shader->Render(commandList);
	}
	for (const auto& object : objects) {
		object->Render(commandList);
	}
}