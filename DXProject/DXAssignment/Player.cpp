#include "stdafx.h"
#include "Camera.h"
#include "Player.h"

CPlayer::CPlayer()
{
	// ī�޶� ����
	camera = std::make_unique<CCamera>();
	camera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	camera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	camera->SetFOVAngle(60.0f);
}

void CPlayer::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);

	CObject::SetPosition(x, y, z);
}


float CPlayer::VelocityToSpeed()
{
	return std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
}

float CPlayer::GetMovingSpeed() const
{
	return moving_speed;
}

void CPlayer::LookAt(XMFLOAT3& lookAt, XMFLOAT3& up)
{
	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view, XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&lookAt), XMLoadFloat3(&up)));

	XMVECTORF32 xmf32vRight = { view._11, view._21, view._31, 0.0f };
	XMVECTORF32 xmf32vUp = { view._12, view._22, view._32, 0.0f };
	XMVECTORF32 xmf32vLook = { view._13, view._23, view._33, 0.0f };

	XMStoreFloat3(&right, XMVector3Normalize(xmf32vRight));
	XMStoreFloat3(&up, XMVector3Normalize(xmf32vUp));
	XMStoreFloat3(&look, XMVector3Normalize(xmf32vLook));
}

void CPlayer::Move(DWORD direction, float distance)
{
	if (direction) {
		XMFLOAT3 shift = XMFLOAT3(0, 0, 0);
		if (direction & DIR_FORWARD) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&look), distance)));
		if (direction & DIR_BACKWARD) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&look), -distance)));
		if (direction & DIR_RIGHT) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&right), distance)));
		if (direction & DIR_LEFT) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&right), -distance)));
		if (direction & DIR_UP) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&up), distance)));
		if (direction & DIR_DOWN) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&up), -distance)));

		Move(shift, true);
	}
}

void CPlayer::Move(XMFLOAT3& shift, bool updateVelocity)
{
	if (updateVelocity) {
		XMStoreFloat3(&velocity, XMVectorAdd(XMLoadFloat3(&velocity), XMLoadFloat3(&shift)));
	}
	else {
		XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&shift)));
		camera->Move(shift);
	}
}

void CPlayer::Move(float x , float y, float z)
{
	XMFLOAT3 v{ x, y, z };
	Move(v, false);
}

void CPlayer::Rotate(float pitch, float yaw, float roll)
{
	camera->Rotate(pitch, yaw, roll);
	if (pitch != 0.0f)
	{
		XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(pitch));
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), rotate));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotate));
	}
	if (yaw != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&up), XMConvertToRadians(yaw));
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), xmmtxRotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), xmmtxRotate));
	}
	if (roll != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&look), XMConvertToRadians(roll));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), xmmtxRotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), xmmtxRotate));
	}

	XMVECTOR xmvLook = XMVector3Normalize(XMLoadFloat3(&look));
	XMVECTOR xmvUp = XMVector3Normalize(XMLoadFloat3(&up));
	XMVECTOR xmvRight = XMVector3Normalize(XMVector3Cross(xmvUp, xmvLook));
	xmvUp = XMVector3Normalize(XMVector3Cross(xmvLook, xmvRight));

	XMStoreFloat3(&right, xmvRight);
	XMStoreFloat3(&up, xmvUp);
	XMStoreFloat3(&look, xmvLook);
}

void CPlayer::AroundRotate(float pitch, float yaw, float roll)
{

	// �̵� �� ������ �ݴ�� rotate
	XMFLOAT3 shiftRotate = { XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll) };
	camera->Move(shiftRotate.x, shiftRotate.y, shiftRotate.z);
	//XMStoreFloat3(&camera_offset, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&shiftRotate)));
	camera->Rotate(-pitch, -yaw, -roll);

}

void CPlayer::SetCameraOffset(XMFLOAT3& cameraOffset)
{
	camera_offset = cameraOffset;
	XMFLOAT3 xmf3CameraPosition;
	XMStoreFloat3(&xmf3CameraPosition, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&camera_offset)));
	camera->SetLookAt(xmf3CameraPosition, position, up);

	camera->GenerateViewMatrix();
}

void CPlayer::SetCameraOffset(XMFLOAT3&& cameraOffset)
{
	camera_offset = cameraOffset;
	XMFLOAT3 xmf3CameraPosition;
	XMStoreFloat3(&xmf3CameraPosition, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&camera_offset)));
	camera->SetLookAt(xmf3CameraPosition, position, up);

	camera->GenerateViewMatrix();
}

void CPlayer::Update(float timeElapsed)
{
	Move(velocity, false);

	camera->Update(this, position, timeElapsed);
	camera->GenerateViewMatrix();

	// ����
	XMVECTOR xmvVelocity = XMLoadFloat3(&velocity);
	XMVECTOR xmvDeceleration = XMVector3Normalize(XMVectorScale(xmvVelocity, -1.0f));
	float length = XMVectorGetX(XMVector3Length(xmvVelocity));
	float deceleration = friction * timeElapsed;
	if (deceleration > length) deceleration = length;
	XMStoreFloat3(&velocity, XMVectorAdd(xmvVelocity, XMVectorScale(xmvDeceleration, deceleration)));
}

void CPlayer::OnUpdateTransform()
{
	world_matrix._11 = right.x; world_matrix._12 = right.y; world_matrix._13 = right.z;
	world_matrix._21 = up.x; world_matrix._22 = up.y; world_matrix._23 = up.z;
	world_matrix._31 = look.x; world_matrix._32 = look.y; world_matrix._33 = look.z;
	world_matrix._41 = position.x; world_matrix._42 = position.y; world_matrix._43 = position.z;
}

void CPlayer::Animate(float elapsedTime)
{
	OnUpdateTransform();

	CObject::Animate(elapsedTime);
}


CAirplanePlayer::CAirplanePlayer()
{
	// mesh �� �÷��̾� ����
	SetPosition(0.0f, 0.0f, 0.0f);
	SetMesh(CCubeMesh());
	SetColor(RGB(0, 0, 255));
	SetCamera(camera);
	SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
}

void CAirplanePlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();

	XMStoreFloat4x4(&world_matrix, XMMatrixMultiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), XMLoadFloat4x4(&world_matrix)));
}


CRollerCosterPlayer::CRollerCosterPlayer()
{
	// mesh �� �÷��̾� ����
	SetPosition(0.0f, 0.0f, 0.0f);
	SetMesh(CCubeMesh());
	SetColor(RGB(255, 0, 255));
	//SetCamera(camera);
	SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
}

void CRollerCosterPlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();

	XMStoreFloat4x4(&world_matrix, XMMatrixMultiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), XMLoadFloat4x4(&world_matrix)));
}

void CNonePlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();
	XMStoreFloat4x4(&world_matrix, XMMatrixMultiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), XMLoadFloat4x4(&world_matrix)));
}

CTankPlayer::CTankPlayer()
{
	// mesh �� �÷��̾� ����
	// �� �Ա�
	SetMesh(CCubeMesh(1.0f, 1.0f, 5.0f, 0.0f, 2.0f, 4.5f));
	// �Ӹ�
	SetMesh(CCubeMesh(4.0f, 2.0f, 4.0f, 0.0f, 2.0f, 0.0f));
	// ��
	SetMesh(CCubeMesh(8.0f, 2.0f, 8.0f));
	//Rotate(90.0f, 90.0f);
	SetPosition(0.0f, 0.0f, 0.0f);
	SetColor(RGB(255, 0, 255));
	SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -20.0f));
	//SetCamera(camera);

}

void CTankPlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();
	//XMStoreFloat4x4(&world_matrix, XMMatrixMultiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), XMLoadFloat4x4(&world_matrix)));
}

void CTankPlayer::Animate(float elapsedTime)
{
	CPlayer::Animate(elapsedTime);

	for (CBulletObject& bullet : bullets) {
		bullet.Animate(elapsedTime);
	}
}

void CTankPlayer::Render(HDC hDCFrameBuffer)
{
	CObject::Render(hDCFrameBuffer);

	for (CBulletObject& bullet : bullets) {
		bullet.Render(hDCFrameBuffer);
	}
}

void CTankPlayer::FireBullet(CObject* pLockedObject)
{

	//std::wstring debugMessage = L"CObject Save cnt: " + std::to_wstring(cnt) + L"\n" + L"size: " + std::to_wstring(meshCount) + L"\n";
	//OutputDebugString(debugMessage.c_str());
	XMFLOAT3 Position = GetPosition();
	Position.y += 2.0f;
	XMFLOAT3 Direction = GetLook();
	XMFLOAT3 FirePosition = Vector3::Add(Position, Vector3::ScalarProduct(Direction, 5.0f, false));
	
	CBulletObject bullet;

	bullet.SetWorldMatrix(world_matrix);

	bullet.SetMesh(CCubeMesh(1.0f, 1.0f, 1.0f));
	bullet.SetFirePosition(FirePosition);
	bullet.SetMovingDirection(Direction);
	bullet.SetRotationSpeed(100.0f);
	bullet.SetMovingSpeed(20.0f);
	bullet.SetColor(RGB(100, 0, 0));

	bullets.emplace_back(bullet);
}
