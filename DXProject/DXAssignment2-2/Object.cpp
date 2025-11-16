#include "stdafx.h"
#include "Shader.h"
#include "Object.h"

CObject::CObject()
{
	XMStoreFloat4x4(&world_matrix, XMMatrixIdentity());
}

void CObject::ReleaseUploadBuffer()
{
	// 정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (mesh) {
		mesh->ReleaseUploadBuffer();
	}
}

void CObject::SetMesh(CMesh* otherMesh)
{
	mesh.reset(otherMesh);
}

void CObject::SetTexture(CTexture* otherTexture)
{
	texture.reset(otherTexture);
}

void CObject::Animate(float elapsedTime)
{

}

void CObject::Rotate(float pitch, float yaw, float roll)
{
}

void CObject::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMFLOAT4X4 TWorldMatrix;
	XMStoreFloat4x4(&TWorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&world_matrix)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &TWorldMatrix, 0);
}

void CObject::Render(ID3D12GraphicsCommandList* commandList)
{
	if (mesh) mesh->Render(commandList);
}