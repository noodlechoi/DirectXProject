#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"

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

// 피킹
void CObject::GenerateRayForPicking(XMVECTOR& pickPosition, XMMATRIX& view, XMVECTOR& pickRayOrigin, XMVECTOR& pickRayDirection)
{
	XMMATRIX xmmtxToModel = XMMatrixInverse(NULL, XMLoadFloat4x4(&world_matrix) * view);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	pickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&xmf3CameraOrigin), xmmtxToModel);
	pickRayDirection = XMVector3TransformCoord(pickPosition, xmmtxToModel);
	pickRayDirection = XMVector3Normalize(pickRayDirection - pickRayOrigin);
}

bool CObject::PickObjectByRayIntersection(XMVECTOR& pickPosition, XMMATRIX& view, float* hitDistance)
{
	if (mesh)
	{
		XMVECTOR pickRayOrigin, pickRayDirection;
		GenerateRayForPicking(pickPosition, view, pickRayOrigin, pickRayDirection);
		return oobb.Intersects(pickRayOrigin, pickRayDirection, *hitDistance);
	}
	return false;
}

void CObject::SetPosition(float x, float y, float z)
{
	world_matrix._41 = x;
	world_matrix._42 = y;
	world_matrix._43 = z;
}
// billboard
void CBillboardObject::Animate(float elapsedTime, CCamera* camera)
{
	XMFLOAT3 cameraPos = camera->GetPos();
	SetLookAt(cameraPos);
}

void CBillboardObject::SetLookAt(XMFLOAT3& target)
{
	XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };
	XMFLOAT3 pos{ world_matrix._41, world_matrix._42, world_matrix._43};
	XMFLOAT3 look{ Vector3::Normalize(Vector3::Subtract(target, pos)) };
	XMFLOAT3 right{ Vector3::CrossProduct(look, up, true) };
	
	world_matrix._11 = right.x; world_matrix._12 = right.y; world_matrix._13 = right.z;
	world_matrix._21 = up.x; world_matrix._22 = up.y; world_matrix._23 = up.z;
	world_matrix._31 = look.x; world_matrix._32 = look.y; world_matrix._33 = look.z;
}
