#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera() 
	: view_matrix{ Matrix4x4::Identity() },
	projection_matrix{ Matrix4x4::Identity() },
	viewport{ 0.0f, 0.0f, float(FRAME_BUFFER_WIDTH), float(FRAME_BUFFER_HEIGHT), 0.0f, 1.0f },
	scissor_rect{ 0, 0, LONG(FRAME_BUFFER_WIDTH), LONG(FRAME_BUFFER_HEIGHT) }
{
}

void CCamera::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	
}

void CCamera::ReleaseShaderVariables()
{

}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMFLOAT4X4 viewMatrix, projectionMatrix;
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&view_matrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projection_matrix)));

	// root signiture index = 1
	commandList->SetGraphicsRoot32BitConstants(1, 16, &viewMatrix, 0);
	commandList->SetGraphicsRoot32BitConstants(1, 16, &projectionMatrix, 16);
}

void CCamera::GenerateViewMatrix(XMFLOAT3 otherPosition, XMFLOAT3 lookAt, XMFLOAT3 otherUp)
{
	position = otherPosition;
	look_at_world = lookAt;
	up = otherUp;

	GenerateViewMatrix();
}

void CCamera::GenerateViewMatrix()
{
	view_matrix = Matrix4x4::LookAtLH(position, look_at_world, up);
}

void CCamera::RegenerateViewMatrix()
{
	look = Vector3::Normalize(look);
	right = Vector3::CrossProduct(up, look, true);
	up = Vector3::CrossProduct(right, look, true);

	view_matrix._11 = right.x; view_matrix._12 = up.x; view_matrix._13 = look.x;
	view_matrix._21 = right.y; view_matrix._22 = up.y; view_matrix._23 = look.y;
	view_matrix._31 = right.z; view_matrix._32 = up.z; view_matrix._33 = look.z;
	view_matrix._41 = -Vector3::DotProduct(position, right);
	view_matrix._42 = -Vector3::DotProduct(position, up);
	view_matrix._43 = -Vector3::DotProduct(position, look);
}

void CCamera::GenerateProjectionMatrix(float nearPlaneDistance, float farPlaneDistance, float aspectRatio, float fovAngle)
{
	projection_matrix = Matrix4x4::PerspectiveFovLH(fovAngle, aspectRatio, nearPlaneDistance, farPlaneDistance);
}

void CCamera::SetViewport(int x, int y, int width, int height, float minZ, float maxZ)
{
	viewport.TopLeftX = float(x);
	viewport.TopLeftY = float(y);
	viewport.Width = float(width);
	viewport.Height = float(height);
	viewport.MinDepth = minZ;
	viewport.MaxDepth = maxZ;
}

void CCamera::SetScissorRect(LONG left, LONG top, LONG right, LONG bottom)
{
	scissor_rect.left = left;
	scissor_rect.top = top;
	scissor_rect.right = right;
	scissor_rect.bottom = bottom;
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* commandList)
{
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissor_rect);
}

void CCamera::Move(const XMFLOAT3& shift)
{
	position.x += shift.x;
	position.y += shift.y;
	position.z += shift.z;
}

void CCamera::Rotate(float pitch, float yaw, float roll)
{
}

void CCamera::Update(XMFLOAT3&, float)
{
}

CFirstPersonCamera::CFirstPersonCamera() : CCamera()
{
}

CThirdPersonCamera::CThirdPersonCamera() : CCamera()
{
}
