#include "stdafx.h"
#include "Scene.h"
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
	for (const auto& shader : shaders) {
		shader->ReleaseUploadBuffers();
	}
}

void CScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	shaders.push_back(std::make_unique<CShader>());
	shaders[0]->CreateShader(device);

	for (int i = 0; i < 1; ++i) {
		objects.push_back(std::make_unique<CGameObject>());
	}
	CTriangleMesh* pTriangleMesh = new CTriangleMesh(device, commandList);
	objects[0]->SetMesh(pTriangleMesh);
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
		shader->Render(commandList);
	}
	for (const auto& object : objects) {
		object->Render(commandList);
	}
}