#include "stdafx.h"
#include "Shader.h"
#include "GameObject.h"

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&world_matrix, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
}


void CGameObject::ReleaseUploadBuffer()
{
	// ���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
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