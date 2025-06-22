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
	if (meshes.data()) {
		for(auto& mesh : meshes)
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
	if (meshes.data()) {
		for (auto& mesh : meshes)
			mesh->Release();
	}
	meshes.push_back(Mesh);
	if (meshes.data()) {
		for (auto& mesh : meshes)
			mesh->AddRef();
	}
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

void CGameObject::Move(XMFLOAT3& direction, float speed)
{
	SetPosition(world_matrix._41 + direction.x * speed, world_matrix._42 + direction.y * speed, world_matrix._43 + direction.z * speed);
}

void CGameObject::Animate(float elapsedTime)
{
	if (moving_speed != 0.0f) Move(moving_direction, moving_speed * elapsedTime);
}

void CGameObject::OnPrepareRender()
{

}

void CGameObject::Render(ID3D12GraphicsCommandList* commandList)
{
	OnPrepareRender();

	UpdateShaderVariables(commandList);

	for (auto& mesh : meshes)
		mesh->Render(commandList);
}

void CRotatingObject::Animate(float elapsedTime)
{
	CGameObject::Rotate(&rotation_axis, rotation_speed * elapsedTime);
}

CRollerCoaster::CRollerCoaster()
{
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

	// 경로까지 이동
	XMFLOAT3 xmf3Movement = Vector3::Normalize(Vector3::Subtract(path[current_index], xmf3Position));
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement, fDistance);
	SetPosition(xmf3Position);
	moving_direction = Vector3::Normalize(Vector3::Add(moving_direction, xmf3Movement));
	if (xmf3Movement.y > 0) {
		SetMovingSpeed(5.0f);
	}
	else if (xmf3Movement.y < 0) {
		SetMovingSpeed(10.0f);
	}

	if (1.0f >= Vector3::Distance(xmf3Position, path[current_index])) {
		current_index = (current_index + 1) % path.size();
		if (0 == current_index) {
			//IsNextScene = true;
		}
	}

	CGameObject::Animate(elapsedTime);
}
CBulletObject::CBulletObject(float effectiveRange) : bullet_effective_range{ effectiveRange }
{
}

CBulletObject::~CBulletObject()
{
	locked_object = nullptr;
}

void CBulletObject::Animate(float elapsedTime)
{
	elapsed_time_after_fire += elapsedTime;

	float distance = moving_speed * elapsedTime;

	if ((elapsed_time_after_fire > lock_delay_time) && locked_object) {
		XMVECTOR xmvPosition = XMLoadFloat3(&GetPosition());

		XMVECTOR lockedObjectPosition = XMLoadFloat3(&locked_object->GetPosition());
		XMVECTOR toLockedObject = XMVector3Normalize(lockedObjectPosition - xmvPosition);

		XMVECTOR movingDirection = XMLoadFloat3(&moving_direction);
		movingDirection = XMVector3Normalize(XMVectorLerp(movingDirection, toLockedObject, 0.25f));
		XMStoreFloat3(&moving_direction, movingDirection);
	}

	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, elapsedTime, 0.0f);
	world_matrix = Matrix4x4::Multiply(mtxRotate, world_matrix);
	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(moving_direction, distance, false);
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
	moving_distance += distance;

	CGameObject::Animate(elapsedTime);
}

void CBulletObject::SetFirePosition(XMFLOAT3 firePosition)
{
	fire_position = firePosition;
	SetPosition(firePosition);
}

void CBulletObject::Reset()
{
	locked_object = nullptr;
	elapsed_time_after_fire = 0;
	moving_distance = 0;
	rotation_angle = 0.0f;
}

CEnemyTank::CEnemyTank()
{
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetMovingSpeed(5.0f);

	std::random_device rd;
	auto rm = std::mt19937(rd());

	auto uid_pos = std::uniform_int_distribution<int>{ -100, 100 };
	SetNextDestination({ (float)uid_pos(rm), 0.0f, (float)uid_pos(rm) });
}

void CEnemyTank::Animate(float elapsedTime)
{
	float distance = moving_speed * elapsedTime;

	XMFLOAT3 xmf3Position = GetPosition();

	// 회전 방향 변경
	XMFLOAT3 normal_direction{ Vector3::Normalize(moving_direction) };
	float dotProduct{ Vector3::DotProduct(GetLook(), normal_direction) };
	if (!IsEqual(dotProduct, 1.0f)) {
		float RotationAngle{ (float)XMConvertToDegrees(acos(dotProduct)) };
		XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, RotationAngle, 0.0f);
		world_matrix = Matrix4x4::Multiply(mtxRotate, world_matrix);
	}

	// 경로까지 이동
	XMFLOAT3 xmf3Movement = Vector3::Normalize(Vector3::Subtract(current_distination, xmf3Position));
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement, distance);
	SetPosition(xmf3Position);
	moving_direction = Vector3::Normalize(Vector3::Add(moving_direction, xmf3Movement));
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);

	if (1.0f >= (xmf3Position.x -  current_distination.x) && 1 >= (xmf3Position.y - current_distination.y)) {
		// 목적지 변경
		current_distination = next_destination;

		std::random_device rd;
		auto rm = std::mt19937(rd());

		auto uid_pos = std::uniform_int_distribution<int>{ -100, 100 };
		SetNextDestination({ (float)uid_pos(rm), 0.0f, (float)uid_pos(rm) });
	}

	//// 중력 적용
	//gravity.Apply(GetPosition(), velocity, elapsedTime);
	//gravity.ResolveCollision(GetPosition(), velocity, terrain->GetHeight(position.x, position.z));

	//// up == height
	//XMVECTOR xmvUp = XMVector3Normalize(XMLoadFloat3(&terrain->GetNormal(position.x, position.z)));
	//XMVECTOR xmvPrevLook = XMVector3Normalize(XMLoadFloat3(&look));
	//// look = 지형 normal과 직교하도록 => 경사로 올라갈 때 적용되도록
	//XMVECTOR xmvLook = XMVector3Normalize(XMVector3Cross(xmvUp, XMVector3Cross(xmvPrevLook, xmvUp)));
	//XMVECTOR xmvRight = XMVector3Normalize(XMVector3Cross(xmvUp, xmvLook));

	//XMStoreFloat3(&up, xmvUp);
	//XMStoreFloat3(&look, xmvLook);
	//XMStoreFloat3(&right, xmvRight);


	CGameObject::Animate(elapsedTime);
}

void CEnemyTank::FireBullet(CGameObject*)
{
}

void CEnemyTank::SetNextDestination(XMFLOAT3 nextDestination)
{
	next_destination = nextDestination;
}

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device*device, ID3D12GraphicsCommandList* commandList, LPCTSTR fileName, int otherWidth, int otherLength, int blockWidth, int blockLength, XMFLOAT3 otherScale, XMFLOAT4 color)
	: width{otherWidth}, length{otherLength}, scale{otherScale}
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
