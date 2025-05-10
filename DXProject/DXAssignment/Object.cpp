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
	rotation_speed{ other.rotation_speed },
	type{other.type}
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
	type = other.type;
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
	rotation_speed{ other.rotation_speed },
	type{ other.type }
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
	type = other.type;
	
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

void CObject::SetPosition(XMFLOAT3&& position)
{
	world_matrix._41 = position.x;
	world_matrix._42 = position.y;
	world_matrix._43 = position.z;
}

XMFLOAT3 CObject::GetPosition()
{
	return(XMFLOAT3(world_matrix._41, world_matrix._42, world_matrix._43));
}

XMFLOAT3 CObject::GetLook()
{
	XMFLOAT3 xmf3LookAt(world_matrix._31, world_matrix._32, world_matrix._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);
	return(xmf3LookAt);
}

XMFLOAT3 CObject::GetUp()
{
	XMFLOAT3 xmf3Up(world_matrix._21, world_matrix._22, world_matrix._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3 CObject::GetRight()
{
	XMFLOAT3 xmf3Right(world_matrix._11, world_matrix._12, world_matrix._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
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

void CObject::Save(std::ostream& out) const
{
	// mesh
	int cnt{};
	size_t meshCount = meshes.size();
	out.write(reinterpret_cast<const char*>(&meshCount), sizeof(size_t));
	for (const auto& mesh : meshes) {
		mesh.Save(out);
		++cnt;
	}

	//std::wstring debugMessage = L"CObject Save cnt: " + std::to_wstring(cnt) + L"\n" + L"size: " + std::to_wstring(meshCount) + L"\n";
	//OutputDebugString(debugMessage.c_str());

	 out.write(reinterpret_cast<const char*>(&world_matrix), sizeof(world_matrix));
	 out.write(reinterpret_cast<const char*>(&color), sizeof(color));
	 out.write(reinterpret_cast<const char*>(&moving_direction), sizeof(moving_direction));
	 out.write(reinterpret_cast<const char*>(&moving_speed), sizeof(moving_speed));
	 out.write(reinterpret_cast<const char*>(&moving_range), sizeof(moving_range));
	 out.write(reinterpret_cast<const char*>(&rotation_axis), sizeof(rotation_axis));
	 out.write(reinterpret_cast<const char*>(&rotation_speed), sizeof(rotation_speed));
}

std::istream& CObject::Load(std::istream& in)
{
	// mesh
	size_t meshCount;
	in.read(reinterpret_cast<char*>(&meshCount), sizeof(size_t));
	meshes.reserve(meshCount);
	for (int i = 0; i < meshCount; ++i) {
		CMesh mesh;
		mesh.Load(in);
		SetMesh(std::move(mesh));
	}

	in.read(reinterpret_cast<char*>(&world_matrix), sizeof(world_matrix));
	in.read(reinterpret_cast<char*>(&color), sizeof(color));
	in.read(reinterpret_cast<char*>(&moving_direction), sizeof(moving_direction));
	in.read(reinterpret_cast<char*>(&moving_speed), sizeof(moving_speed));
	in.read(reinterpret_cast<char*>(&moving_range), sizeof(moving_range));
	in.read(reinterpret_cast<char*>(&rotation_axis), sizeof(rotation_axis));
	in.read(reinterpret_cast<char*>(&rotation_speed), sizeof(rotation_speed));

	return in;
}


CRollerCoaster::CRollerCoaster()
{
	SetType(eTYPE::RollerCoaster);

	path = {
	   { 0.0f, 0.0f, 0.0f },
		{ 20.0f, 0.0f, 10.0f },
		{ 30.0f, 10.0f, 40.0f },
		{ 40.0f, 30.0f, 80.0f },
		{ 40.0f, 40.0f, 90.0f },
		{ 30.0f, 0.0f, 100.0f },
		{ 0.0f, 10.0f, 60.0f },
		{ -50.0f, -10.0f, 50.0f },
	};
}

void CRollerCoaster::Animate(float elapsedTime)
{
	float fDistance = moving_speed * elapsedTime;

	XMFLOAT3 xmf3Position = GetPosition();

	// 회전 방향 변경
	XMFLOAT3 dotProduct{ Vector3::DotProduct(GetRight(), moving_direction),
		Vector3::DotProduct(GetUp(), moving_direction),
		Vector3::DotProduct(GetLook(), moving_direction) 
	};
	// 각 좌표축에 대한 각도 구하기(일직선 상에 있으면 구하지 않기)
	XMFLOAT3 RotationAngle{ ::IsEqual(dotProduct.x, 1.0f) ? 0.0f : (float)XMConvertToDegrees(acos(dotProduct.x)),
		::IsEqual(dotProduct.y, 1.0f) ? 0.0f : (float)XMConvertToDegrees(acos(dotProduct.y)),
		::IsEqual(dotProduct.z, 1.0f) ? 0.0f : (float)XMConvertToDegrees(acos(dotProduct.z)) 
	};

	// 회전 시 객체가 사라짐... -nan(ind) 나옴
	//if(0 != RotationAngle.x || 0 != RotationAngle.y || 0 != RotationAngle.z)
	//	Rotate(RotationAngle.x, RotationAngle.y, RotationAngle.z);

	// 경로까지 이동
	XMFLOAT3 xmf3Movement = Vector3::Normalize(Vector3::Subtract(path[current_index], xmf3Position));
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement, fDistance);
	SetPosition(xmf3Position);
	moving_direction = Vector3::Normalize(Vector3::Add(moving_direction, xmf3Movement));

	//// 경로까지 도달하면
	//std::wstring debugMessage2 = L"RotationAngle: " + std::to_wstring(RotationAngle.x) + std::to_wstring(RotationAngle.y) + std::to_wstring(RotationAngle.z) + L"\n";
	//OutputDebugString(debugMessage2.c_str());
	if (1.0f >= Vector3::Distance(xmf3Position, path[current_index])) {
		current_index = (current_index + 1) % path.size();
	}

	CObject::Animate(elapsedTime);
}
