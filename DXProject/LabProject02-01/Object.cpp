#include "stdafx.h"
#include "Camera.h"
#include "GraphicsPipeline.h"
#include "Object.h"

CObject::CObject() :
	world_matrix{Matrix4x4::Identity()},
	moving_direction{XMFLOAT3(0.0f,0.0f,1.0f)},
	rotation_axis{XMFLOAT3(0.0f, 1.0f, 0.0f)}
{
}

void CObject::SetMesh(CMesh mesh)
{
	meshes.emplace_back(mesh);
	BoundingBox meshesAABB = MergeMeshesBoundingBox();
	BoundingOrientedBox::CreateFromBoundingBox(OBB, meshesAABB);
}

void CObject::SetPosition(float x, float y, float z)
{
	world_matrix._41 = x;
	world_matrix._42 = y;
	world_matrix._43 = z;
}

void CObject::SetPosition(XMFLOAT3& position)
{
	world_matrix._41 = position.x;
	world_matrix._42 = position.y;
	world_matrix._43 = position.z;
}

void CObject::Move(XMFLOAT3& direction, float speed)
{
	SetPosition(world_matrix._41 + direction.x * speed, world_matrix._42 + direction.y * speed, world_matrix._43 + direction.z * speed);
}

void CObject::Rotate(float pitch, float yaw, float roll)
{
	XMFLOAT4X4 rotate = Matrix4x4::RotationYawPitchRoll(pitch, yaw, roll);
	world_matrix = Matrix4x4::Multiply(rotate, world_matrix);
}

void CObject::Rotate(XMFLOAT3& rotationAxis, float angle)
{
	XMFLOAT4X4 rotate = Matrix4x4::RotationAxis(rotationAxis, angle);
	world_matrix = Matrix4x4::Multiply(rotate, world_matrix);
}

BoundingBox CObject::MergeMeshesBoundingBox()
{
	BoundingBox mergedAABB;
	bool isInited{ false };

	for (const CMesh& mesh : meshes) {
		if (!isInited) {
			mergedAABB = mesh.AABB;
			isInited = true;
		}
		else {
			BoundingBox::CreateMerged(mergedAABB, mergedAABB, mesh.AABB);
		}
	}

	return mergedAABB;
}

void CObject::UpdateBoundingBox()
{
	OBB.Transform(OBB, XMLoadFloat4x4(&world_matrix));
	XMStoreFloat4(&OBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&OBB.Orientation)));
}

void CObject::GenerateRayForPicking(XMVECTOR& pickPosition, XMMATRIX& viewMatrix, XMVECTOR& pickRayOrigin, XMVECTOR& pickRayDirection)
{
	XMMATRIX toModelMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&world_matrix) * viewMatrix);

	XMFLOAT3 cameraOrigin(0.0f, 0.0f, 0.0f);
	pickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&cameraOrigin), toModelMatrix);
	pickRayDirection = XMVector3TransformCoord(pickPosition, toModelMatrix);
	pickRayDirection = XMVector3Normalize(pickRayDirection - pickRayOrigin);
}

int CObject::PickObjectByRayIntersection(XMVECTOR& pickPosition, XMMATRIX& viewMatrix, float& hitDistance)
{
	size_t intersectedNum{};
	if (meshes.size()) {
		XMVECTOR pickRayOrigin, pickRayDirection;
		GenerateRayForPicking(pickPosition, viewMatrix, pickRayOrigin, pickRayDirection);
		if (OBB.Intersects(pickRayOrigin, pickRayDirection, hitDistance)) {
			for (CMesh& mesh : meshes) {
				intersectedNum = mesh.CheckRayIntersection(pickRayOrigin, pickRayDirection, hitDistance);
			}
		}
	}

	return intersectedNum;
}


void CObject::Animate(float timeElapsed)
{
	if (rotation_speed != 0.0f) Rotate(rotation_axis, rotation_speed * timeElapsed);
	if (moving_speed != 0.0f) Move(moving_direction, moving_speed * timeElapsed);

	UpdateBoundingBox();
}

void CObject::Render(HDC hDCFrameBuffer ,CCamera& camera)
{
	if(camera.IsInFrustum(OBB)) {
		CGraphicsPipeline::SetWorldTransform(world_matrix);

		HPEN hPen = ::CreatePen(PS_SOLID, 0, color);
		HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

		for (CMesh& m : meshes) {
			m.Render(hDCFrameBuffer);
		}

		::SelectObject(hDCFrameBuffer, hOldPen);
		::DeleteObject(hPen);
	}
}

