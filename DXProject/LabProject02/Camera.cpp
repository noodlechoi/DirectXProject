#include "stdafx.h"
#include "Camera.h"

void CViewport::SetViewport(int Left, int Top, int Width, int Height)
{
	left = Left;
	top = Top;
	width = Width;
	height = Height;
}

CCamera::CCamera() 
	: view_matrix{ Matrix4x4::Identity()},
	perspective_project_matrix{ Matrix4x4::Identity() },
	view_perspective_project_matrix{ Matrix4x4::Identity() }
{
}

void CCamera::SetFOVAngle(float FOV)
{
	fov_angle = FOV;
	project_distance = float(1.0f / tan(XMConvertToRadians(FOV * 0.5f)));
}

void CCamera::GenerateViewMatrix()
{
	// 카메라 벡터 구하기
	XMVECTOR mvLook = XMVector3Normalize(XMLoadFloat3(&look));
	XMVECTOR mvUp = XMVector3Normalize(XMLoadFloat3(&up));
	// 외적 값 nomalized
	XMVECTOR mvRight = XMVector3Normalize(XMVector3Cross(mvUp, mvLook));
	mvUp = XMVector3Normalize(XMVector3Cross(mvLook, mvRight));

	// 멤버에 저장
	XMStoreFloat3(&look, mvLook);
	XMStoreFloat3(&up, mvUp);
	XMStoreFloat3(&right, mvRight);

	// 행렬에 카메라 벡터 넣기
	view_matrix._11 = right.x; view_matrix._12 = up.x; view_matrix._13 = look.x;
	view_matrix._21 = right.y; view_matrix._22 = up.y; view_matrix._23 = look.y;
	view_matrix._31 = right.z; view_matrix._32 = up.z; view_matrix._33 = look.z;

	// 위치 벡터 넣기
	XMVECTOR mvPosition = XMLoadFloat3(&position);
	view_matrix._41 = -XMVectorGetX(XMVector3Dot(mvPosition, mvRight));
	view_matrix._42 = -XMVectorGetX(XMVector3Dot(mvPosition, mvUp));
	view_matrix._43 = -XMVectorGetX(XMVector3Dot(mvPosition, mvLook));

	XMStoreFloat4x4(&view_perspective_project_matrix, XMMatrixMultiply(XMLoadFloat4x4(&view_matrix), XMLoadFloat4x4(&perspective_project_matrix)));
}

void CCamera::GeneratePerspectiveProjectionMatrix(float nearPlaneDistance, float farPlaneDistance, float FovAngle)
{
	float fAspectRatio = (float(viewport.width) / float(viewport.height));
	XMStoreFloat4x4(&perspective_project_matrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(FovAngle), fAspectRatio, nearPlaneDistance, farPlaneDistance));
}

void CCamera::SetViewport(int Left, int Top, int Width, int Height)
{
	viewport.SetViewport(Left, Top, Width, Height);
	aspect_ratio = float(viewport.width) / float(viewport.height);
}

void CCamera::SetLookAt(XMFLOAT3& lookAt, XMFLOAT3& Up)
{
	SetLookAt(position, lookAt, Up);
}

void CCamera::SetLookAt(XMFLOAT3& Position, XMFLOAT3& lookAt, XMFLOAT3& Up)
{
	position = Position;
	XMStoreFloat4x4(&view_matrix, XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&lookAt), XMLoadFloat3(&Up)));

	XMVECTORF32 xm32vRight = { view_matrix._11, view_matrix._21, view_matrix._31, 0.0f };
	XMVECTORF32 xm32vUp = { view_matrix._12, view_matrix._22, view_matrix._32, 0.0f };
	XMVECTORF32 xm32vLook = { view_matrix._13, view_matrix._23, view_matrix._33, 0.0f };

	XMStoreFloat3(&right, XMVector3Normalize(xm32vRight));
	XMStoreFloat3(&up, XMVector3Normalize(xm32vUp));
	XMStoreFloat3(&look, XMVector3Normalize(xm32vLook));
}

void CCamera::Move(XMFLOAT3& shift)
{
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&shift)));
}

void CCamera::Move(XMFLOAT3&& shift)
{
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&shift)));
}

void CCamera::Move(float x , float y, float z)
{
	Move(XMFLOAT3(x, y, z));
}

void CCamera::Rotate(float pitch, float yaw, float roll)
{
	if (pitch != 0.0f)
	{
		XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(pitch));
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), rotate));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotate));
	}
	if (yaw != 0.0f)
	{
		XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&up), XMConvertToRadians(yaw));
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), rotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), rotate));
	}
	if (roll != 0.0f)
	{
		XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&look), XMConvertToRadians(roll));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), rotate));
	}
}

//void CCamera::Update(std::unique_ptr<CPlayer> player, XMFLOAT3& lookAt, float timeElapsed)
//{
//	XMMATRIX rotate;
//	rotate.r[0] = XMVectorSet(player.get()m_xmf3Right.x, player->m_xmf3Right.y, player->m_xmf3Right.z, 0.0f);
//	rotate.r[1] = XMVectorSet(player->m_xmf3Up.x, player->m_xmf3Up.y, player->m_xmf3Up.z, 0.0f);
//	rotate.r[2] = XMVectorSet(player->m_xmf3Look.x, player->m_xmf3Look.y, player->m_xmf3Look.z, 0.0f);
//	rotate.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//	XMVECTOR xmvPosition = XMLoadFloat3(&position);
//	XMVECTOR xmvOffset = XMVector3TransformCoord(XMLoadFloat3(&player->m_xmf3CameraOffset), rotate);
//	XMVECTOR xmvNewPosition = XMVectorAdd(XMLoadFloat3(&player->position), xmvOffset);
//	XMVECTOR xmvDirection = XMVectorSubtract(xmvNewPosition, xmvPosition);
//
//	float fLength = XMVectorGetX(XMVector3Length(xmvDirection));
//	xmvDirection = XMVector3Normalize(xmvDirection);
//
//	float fTimeLagScale = fTimeElapsed * 4.0f;
//	float fDistance = fLength * fTimeLagScale;
//	if (fDistance > fLength) fDistance = fLength;
//	if (fLength < 0.01f) fDistance = fLength;
//	if (fDistance > 0)
//	{
//		XMStoreFloat3(&position, XMVectorAdd(xmvPosition, XMVectorScale(xmvDirection, fDistance)));
//		SetLookAt(player->position, player->m_xmf3Up);
//	}
//}

