#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer(float width, float height) : camera{ std::make_unique<CCamera>() }
{
	// 카메라 객체 생성
	camera->SetViewport(0, 0, width, height);
	camera->SetScissorRect(0, 0, width, height);
	camera->GenerateProjectionMatrix(1.0f, 500.0f, float(width) / float(height), 90.0f);
	camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void CPlayer::Render(ID3D12GraphicsCommandList* commandList)
{
	camera->SetViewportsAndScissorRects(commandList);
	camera->UpdateShaderVariables(commandList);
}
