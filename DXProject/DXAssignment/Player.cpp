#include "stdafx.h"
#include "Camera.h"
#include "Player.h"

CPlayer::CPlayer()
{
	// 카메라 설정
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
	// mesh 및 플레이어 설정
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
	// mesh 및 플레이어 설정
	SetPosition(0.0f, 0.0f, 0.0f);
	SetMesh(CCubeMesh());
	SetColor(RGB(255, 0, 255));
	SetCamera(camera);
	SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
}

void CRollerCosterPlayer::OnUpdateTransform()
{
	// 롤러코스터 라인에 따라 이동

	CPlayer::OnUpdateTransform();

	XMStoreFloat4x4(&world_matrix, XMMatrixMultiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), XMLoadFloat4x4(&world_matrix)));
}

void CNonePlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();
	XMStoreFloat4x4(&world_matrix, XMMatrixMultiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), XMLoadFloat4x4(&world_matrix)));
}
