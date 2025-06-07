#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer(CCamera* otherCamera, float width, float height)
{
	camera.reset(otherCamera);
	camera->SetViewport(0, 0, width, height);
	camera->SetScissorRect(0, 0, width, height);
	camera->GenerateProjectionMatrix(1.0f, 500.0f, float(width) / float(height), 90.0f);
	camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void CPlayer::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	CGameObject::CreateShaderVariables(device, commandList);
	if (camera) camera->CreateShaderVariables(device, commandList);
}

void CPlayer::Move(const XMFLOAT3& shift, bool updateVelocity)
{
	if (updateVelocity) {
		velocity = Vector3::Add(velocity, shift);
	}
	else {
		position = Vector3::Add(position, shift);
		if (camera) camera->Move(shift);
	}
}

void CPlayer::Rotate(float pitch, float yaw, float roll)
{
	if (camera) camera->Rotate(pitch, yaw, roll);
}

void CPlayer::Render(ID3D12GraphicsCommandList* commandList)
{
	camera->SetViewportsAndScissorRects(commandList);
	camera->UpdateShaderVariables(commandList);

	CGameObject::Render(commandList);
}

CSpaceShipPlayer::CSpaceShipPlayer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, float width, float height) : CPlayer(new CThirdPersonCamera, width, height)
{
	std::shared_ptr<CMesh> cubeMesh = std::make_shared<CAirPlaneMeshDiffused>(device, commandList);
	SetMesh(cubeMesh);
}
