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
	shaders.push_back(std::make_unique<CTextureShader>());
	shaders[0]->CreateShader(device);

	for (int i = 0; i < 1; ++i) {
		objects.push_back(std::make_unique<CObject>());
	}
	//CTriangleMesh* mesh= new CTriangleMesh(device, commandList);
	CRectangleMesh* mesh= new CRectangleMesh(device, commandList);
	objects[0]->SetMesh(mesh);
	CTexture* tex = new CTexture(std::string("stone"));
	tex->CreateTextureResource(device, commandList, std::wstring(L"../Image/Stone01.dds"));
	objects[0]->SetTexture(tex);
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