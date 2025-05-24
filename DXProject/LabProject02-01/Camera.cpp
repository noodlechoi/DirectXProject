#include "stdafx.h"
#include "Player.h"
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
	view_perspective_project_matrix{ Matrix4x4::Identity() },
	inverse_view_matrix{Matrix4x4::Identity()}
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
	look = Vector3::Normalize(look);
	right = Vector3::Normalize(Vector3::CrossProduct(up, look));
	up = Vector3::Normalize(Vector3::CrossProduct(look, right));

	// 행렬에 카메라 벡터 넣기
	view_matrix._11 = right.x; view_matrix._12 = up.x; view_matrix._13 = look.x;
	view_matrix._21 = right.y; view_matrix._22 = up.y; view_matrix._23 = look.y;
	view_matrix._31 = right.z; view_matrix._32 = up.z; view_matrix._33 = look.z;
	view_matrix._41 = -Vector3::DotProduct(position, right);
	view_matrix._42 = -Vector3::DotProduct(position, up);
	view_matrix._43 = -Vector3::DotProduct(position, look);

	// 뷰 변환 행렬 * 투영 변환 행렬
	view_perspective_project_matrix = Matrix4x4::Multiply(view_matrix, perspective_project_matrix);

	// inverse matrix
	inverse_view_matrix = Matrix4x4::Inverse(view_matrix);

	// 카메라 frustum을 월드 frustum으로 변환 => 오브젝트는 월드 좌표계에 있기 때문?
	frustum_view.Transform(frustum_world, XMLoadFloat4x4(&inverse_view_matrix));
}

void CCamera::GeneratePerspectiveProjectionMatrix(float nearPlaneDistance, float farPlaneDistance, float FovAngle)
{
	float fAspectRatio = (float(viewport.width) / float(viewport.height));
	// 투영 변환 행렬 구하기
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(FovAngle), fAspectRatio, nearPlaneDistance, farPlaneDistance);
	XMStoreFloat4x4(&perspective_project_matrix, projection);

	// 투영 변환 행렬로부터 frustum 생성
	BoundingFrustum::CreateFromMatrix(frustum_view, projection);
}

void CCamera::SetViewport(int Left, int Top, int Width, int Height)
{
	viewport.SetViewport(Left, Top, Width, Height);
	aspect_ratio = float(viewport.width) / float(viewport.height);
}

void CCamera::SetLookAt(XMFLOAT3& lookAt, XMFLOAT3& Up)
{
	XMFLOAT4X4 view = Matrix4x4::LookAtLH(position, lookAt, Up);
	right = Vector3::Normalize(XMFLOAT3(view._11, view._21, view._31));
	up = Vector3::Normalize(XMFLOAT3(view._12, view._22, view._32));
	look = Vector3::Normalize(XMFLOAT3(view._13, view._23, view._33));
}

void CCamera::SetLookAt(XMFLOAT3& Position, XMFLOAT3& lookAt, XMFLOAT3& Up)
{
	position = Position;
	view_matrix = Matrix4x4::LookAtLH(position, lookAt, Up);
	right = Vector3::Normalize(XMFLOAT3(view_matrix._11, view_matrix._21, view_matrix._31));
	up = Vector3::Normalize(XMFLOAT3(view_matrix._12, view_matrix._22, view_matrix._32));
	look = Vector3::Normalize(XMFLOAT3(view_matrix._13, view_matrix._23, view_matrix._33));
}

void CCamera::Move(XMFLOAT3& shift)
{
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&shift)));
}

void CCamera::Move(float x , float y, float z)
{
	Move(XMFLOAT3(x, y, z));
}

void CCamera::Rotate(float pitch, float yaw, float roll)
{
	if (pitch != 0.0f) {
		XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(pitch));
		look = Vector3::TransformNormal(look, rotate);
		up = Vector3::TransformNormal(up, rotate);
	}
	if (yaw != 0.0f) {
		XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&up), XMConvertToRadians(yaw));
		look = Vector3::TransformNormal(look, rotate);
		right = Vector3::TransformNormal(right, rotate);
	}
	if (roll != 0.0f) {
		XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&look), XMConvertToRadians(roll));
		right = Vector3::TransformNormal(right, rotate);
		up = Vector3::TransformNormal(up, rotate);
	}
}

void CCamera::Update(CPlayer& player, XMFLOAT3& lookAt, float timeElapsed)
{
	XMFLOAT4X4 rotate = Matrix4x4::Identity();
	rotate._11 = player.right.x; rotate._21 = player.up.x; rotate._31 = player.look.x;
	rotate._12 = player.right.y; rotate._22 = player.up.y; rotate._32 = player.look.y;
	rotate._13 = player.right.z; rotate._23 = player.up.z; rotate._33 = player.look.z;

	// 플레이어 회전에 따른 오프셋 벡터 구하기
	XMFLOAT3 Offset = Vector3::TransformCoord(player.camera_offset, rotate);
	XMFLOAT3 Position = Vector3::Add(player.position, Offset);
	// 카메라 위치와 이동할 위치 차이
	XMFLOAT3 Direction = Vector3::Subtract(Position, position);
	float length = Vector3::Length(Direction);
	Direction = Vector3::Normalize(Direction);

	float timeLagScale = timeElapsed * (1.0f / 0.25f);
	float distance = length * timeLagScale;
	if (distance > length) distance = length;
	if (length < 0.01f) distance = length;	// 너무 작으면 그냥 이동
	if (distance > 0)
	{
		position = Vector3::Add(position, Direction, distance);
		SetLookAt(player.position, player.up);
	}
}

bool CCamera::IsInFrustum(BoundingOrientedBox& boundingBox)
{
	return frustum_world.Intersects(boundingBox);
}
