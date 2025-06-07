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

void CCamera::GenerateViewMatrix(XMFLOAT3 position, XMFLOAT3 lookAt, XMFLOAT3 up)
{
	view_matrix = Matrix4x4::LookAtLH(position, lookAt, up);
}

void CCamera::GenerateProjectionMatrix(float nearPlaneDistance, float farPlaneDistance, float aspectRatio, float fovAngle)
{
	projection_matrix = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fovAngle), aspectRatio, nearPlaneDistance, farPlaneDistance);
}

void CCamera::SetViewport(int x, int y, int width, int height, float minZ, float maxZ)
{
	viewport.TopLeftX = (float)x;
	viewport.TopLeftY = (float)y;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
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
