#include "stdafx.h"
#include "Shader.h"
#include "GameObject.h"

CGameObject::CGameObject() : world_matrix{Matrix4x4::Identity()}
{
}

CGameObject::~CGameObject()
{
}


void CGameObject::ReleaseUploadBuffer()
{
	// 정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (mesh) {
		mesh->ReleaseUploadBuffer();
	}
}

void CGameObject::SetMesh(CMesh* Mesh)
{
	if (mesh) mesh->Release();
	mesh.reset(Mesh);
	if (mesh) mesh->AddRef();
}

void CGameObject::SetShader(CShader*)
{

}

void CGameObject::Animate(float elapsedTime)
{

}

void CGameObject::OnPrepareRender()
{

}

void CGameObject::Render(ID3D12GraphicsCommandList* commandList)
{
	OnPrepareRender();

	if (mesh) mesh->Render(commandList);
}