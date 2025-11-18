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
	for(const auto& mesh : meshes)
		mesh->ReleaseUploadBuffer();
}

void CObject::SetMesh(std::shared_ptr<CMesh>& otherMesh)
{
	meshes.push_back(otherMesh);
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
	for (const auto& mesh : meshes)
		mesh->Render(commandList);
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
	if (!meshes.empty())
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

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, LPCTSTR fileName, int otherWidth, int otherLength, int blockWidth, int blockLength, XMFLOAT3 otherScale, XMFLOAT4 color)
	: width{ otherWidth }, length{ otherLength }, scale{ otherScale }
{
	int xQuadsPerBlock{ blockWidth - 1 };
	int zQuadsPerBlock{ blockLength - 1 };

	height_map_image = std::make_unique<CHeightMapImage>(fileName, width, length, scale);

	long xBlocks = (width - 1) / xQuadsPerBlock;
	long zBlocks = (length - 1) / zQuadsPerBlock;

	std::shared_ptr<CMesh> heightMapMesh;
	for (int z = 0, zStart = 0; z < zBlocks; ++z) {
		for (int x = 0, xStart = 0; x < xBlocks; ++x) {
			xStart = x * (blockWidth - 1);
			zStart = z * (blockLength - 1);
			heightMapMesh = std::make_shared<CHeightMapGridMesh>(device, commandList, xStart, zStart, blockWidth, blockLength, scale, color, height_map_image.get());
			SetMesh(heightMapMesh);
		}
	}
}
