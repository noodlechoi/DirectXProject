#include "stdafx.h"
#include "Shader.h"
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

void CGameObject::CreateShaderVariables(ID3D12Device*, ID3D12GraphicsCommandList*)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMFLOAT4X4 TWorldMatrix;
	XMStoreFloat4x4(&TWorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&world_matrix)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &TWorldMatrix, 0);
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::SetMesh(std::shared_ptr<CMesh>& Mesh)
{
	if (mesh) mesh->Release();
	mesh = Mesh;
	if (mesh) mesh->AddRef();
}

void CGameObject::SetShader(std::shared_ptr<CShader>& otherShader)
{
	shader = otherShader;
}

XMFLOAT3 CGameObject::GetLook() const
{
	return Vector3::Normalize(XMFLOAT3(world_matrix._31, world_matrix._32, world_matrix._33));
}

XMFLOAT3 CGameObject::GetUp() const
{
	return Vector3::Normalize(XMFLOAT3(world_matrix._21, world_matrix._22, world_matrix._23));
}

XMFLOAT3 CGameObject::GetRight() const
{
	return Vector3::Normalize(XMFLOAT3(world_matrix._11, world_matrix._12, world_matrix._13));
}

void CGameObject::SetPosition(float x, float y, float z)
{
	world_matrix._41 = x;
	world_matrix._42 = y;
	world_matrix._43 = z;
}

void CGameObject::Rotate(XMFLOAT3* axis, float angle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(axis), XMConvertToRadians(angle));
	world_matrix = Matrix4x4::Multiply(mtxRotate, world_matrix);
}

void CGameObject::Rotate(float pitch, float yaw, float rool)
{
	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(rool));
	world_matrix = Matrix4x4::Multiply(rotateMatrix, world_matrix);
}

void CGameObject::Move(float elapsedTime)
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

	UpdateShaderVariables(commandList);

	if (shader)	shader->Render(commandList);

	if (mesh) mesh->Render(commandList);
}

void CRotatingObject::Animate(float elapsedTime)
{
	CGameObject::Rotate(&rotation_axis, rotation_speed * elapsedTime);
}
