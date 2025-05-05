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

CObject::~CObject()
{
	if (meshes.data()) {
		for (auto& m : meshes) {
			m.Release();
		}
	}
}

CObject::CObject(const CObject& other) :
	is_active{ other.is_active },
	world_matrix{ other.world_matrix },
	color{ other.color },
	moving_direction{ other.moving_direction },
	moving_speed{ other.moving_speed },
	moving_range{ other.moving_range },
	rotation_axis{ other.rotation_axis },
	rotation_speed{ other.rotation_speed }
{
	if (other.meshes.data()) {
		meshes.reserve(other.meshes.size());
		meshes = other.meshes;
	}
	else {
		meshes.clear();
	}
}

CObject& CObject::operator=(const CObject& other)
{
	if (this == &other) return *this;

	is_active = other.is_active;
	world_matrix = other.world_matrix;
	color = other.color;
	moving_direction = other.moving_direction;
	moving_speed = other.moving_speed;
	moving_range = other.moving_range;
	rotation_axis = other.rotation_axis;
	rotation_speed = other.rotation_speed;
	if (other.meshes.data()) {
		meshes.resize(other.meshes.size());
		meshes = other.meshes;
	}
	else {
		meshes.clear();
	}

	return *this;
}

CObject::CObject( CObject&& other) :
	is_active{ other.is_active },
	world_matrix{ other.world_matrix },
	color{ other.color },
	moving_direction{ other.moving_direction },
	moving_speed{ other.moving_speed },
	moving_range{ other.moving_range },
	rotation_axis{ other.rotation_axis },
	rotation_speed{ other.rotation_speed }
{
	meshes.reserve(other.meshes.size());
	meshes = std::move(other.meshes);
}

CObject& CObject::operator=( CObject&& other)
{
	if (this == &other) return *this;

	is_active = other.is_active;
	world_matrix = other.world_matrix;
	color = other.color;
	moving_direction = other.moving_direction;
	moving_speed = other.moving_speed;
	moving_range = other.moving_range;
	rotation_axis = other.rotation_axis;
	rotation_speed = other.rotation_speed;
	
	meshes.resize(other.meshes.size());
	meshes = std::move(other.meshes);

	return *this;
}

void CObject::SetMesh(CMesh&& mesh)
{
	mesh.AddRef();
	meshes.push_back(mesh);
}

void CObject::SetMesh(CMesh& mesh)
{
	mesh.AddRef();
	meshes.push_back(mesh);
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
	XMMATRIX xmmtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll));
	XMStoreFloat4x4(&world_matrix, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&world_matrix)));
}

void CObject::Rotate(XMFLOAT3& rotationAxis, float angle)
{
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&rotationAxis), XMConvertToRadians(angle));
	XMStoreFloat4x4(&world_matrix, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&world_matrix)));
}

void CObject::Animate(float elapsedTime)
{
	if (rotation_speed != 0.0f) Rotate(rotation_axis, rotation_speed * elapsedTime);
	if (moving_speed != 0.0f) Move(moving_direction, moving_speed * elapsedTime);
}

void CObject::Render(HDC hDCFrameBuffer)
{
	if (meshes.data()) {
		CGraphicsPipeline::SetWorldTransform(world_matrix);

		HPEN hPen = ::CreatePen(PS_SOLID, 0, color);
		HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

		for (const auto& m : meshes) {
			m.Render(hDCFrameBuffer);
		}

		::SelectObject(hDCFrameBuffer, hOldPen);
		::DeleteObject(hPen);
	}
}
