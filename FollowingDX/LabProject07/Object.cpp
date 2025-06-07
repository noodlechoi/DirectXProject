#include "stdafx.h"
#include "Object.h"

CGameObject::CGameObject() : world_matrix{Matrix4x4::Identity()}
{
}

CGameObject::~CGameObject()
{
}


void CGameObject::ReleaseUploadBuffers()
{
	// 정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (mesh) {
		mesh->ReleaseUploadBuffers();
	}
}

void CGameObject::SetMesh(CMesh* Mesh)
{
	if (mesh) mesh->Release();
	mesh.reset(Mesh);
	if (mesh) mesh->AddRef();
}

void CGameObject::SetShader(CShader* otherShader)
{
	shader.reset(otherShader);
}

void CGameObject::Rotate(XMFLOAT3* axis, float angle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(axis), XMConvertToRadians(angle));
	world_matrix = Matrix4x4::Multiply(mtxRotate, world_matrix);
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

	if (shader) {
		shader->UpdateShaderVariables(commandList, &world_matrix);
		shader->Render(commandList);
	}

	if (mesh) mesh->Render(commandList);
}

void CRotatingObject::Animate(float elapsedTime)
{
	CGameObject::Rotate(&rotation_axis, rotation_speed * elapsedTime);
}
