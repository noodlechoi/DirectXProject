#include "stdafx.h"
#include "Camera.h"
#include "GraphicsPipeline.h"
#include "Player.h"
#include "Scene.h"

template <typename T, typename Y>
CScene::CScene(size_t objectNum, T&& manager, Y&& Player)
{
	input_manager.reset(manager.release());
	player.reset(Player.release());
}


void CScene::Animate(float elapsedTime)
{
	player->Animate(elapsedTime);

	for (auto& object : objects) {
		object->Animate(elapsedTime);
	}
}

void CScene::Render(HDC hDCFrameBuffer)
{
	CGraphicsPipeline::SetViewport(player->camera->viewport);
	CGraphicsPipeline::SetViewPerspectiveProjectTransform(player->camera->view_perspective_project_matrix);

	player->Render(hDCFrameBuffer, *player->camera);

	for (auto& object : objects) {
		object->Render(hDCFrameBuffer, *player->camera);
	}
}

LRESULT CScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}

CSpaceShipScene::CSpaceShipScene() : CScene(5, std::make_unique<CSpaceShipInputManager>(), std::make_unique<CAirplanePlayer>())
{
}

void CSpaceShipScene::BuildObjects()
{

	CCubeMesh cube{ 4.0f, 4.0f, 4.0f };
	objects[0] = CObject();
	objects[0].SetMesh(cube);
	objects[0].SetColor(RGB(255, 0, 0));
	objects[0].SetPosition(-13.5f, 0.0f, +14.0f);
	objects[0].SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	objects[0].SetRotationSpeed(90.0f);
	objects[0].SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
	objects[0].SetMovingSpeed(0.0f);

	objects[1] = CObject();
	objects[1].SetMesh(cube);
	objects[1].SetColor(RGB(0, 0, 255));
	objects[1].SetPosition(+13.5f, 0.0f, +14.0f);
	objects[1].SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	objects[1].SetRotationSpeed(180.0f);
	objects[1].SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	objects[1].SetMovingSpeed(0.0f);

	objects[2] = CObject();
	objects[2].SetMesh(cube);
	objects[2].SetColor(RGB(0, 255, 0));
	objects[2].SetPosition(0.0f, +5.0f, 20.0f);
	objects[2].SetRotationAxis(XMFLOAT3(1.0f, 0.0f, 1.0f));
	objects[2].SetRotationSpeed(30.15f);
	objects[2].SetMovingDirection(XMFLOAT3(1.0f, -1.0f, 0.0f));
	objects[2].SetMovingSpeed(0.0f);

	objects[3] = CObject();
	objects[3].SetMesh(cube);
	objects[3].SetColor(RGB(0, 255, 255));
	objects[3].SetPosition(0.0f, 0.0f, 40.0f);
	objects[3].SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
	objects[3].SetRotationSpeed(40.6f);
	objects[3].SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));
	objects[3].SetMovingSpeed(0.0f);

	objects[4] = CObject();
	objects[4].SetMesh(cube);
	objects[4].SetColor(RGB(128, 0, 255));
	objects[4].SetPosition(10.0f, 10.0f, 50.0f);
	objects[4].SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	objects[4].SetRotationSpeed(50.06f);
	objects[4].SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 1.0f));
	objects[4].SetMovingSpeed(0.0f);
}

void CSpaceShipScene::ProcessInput(HWND& hwnd, float timeElapsed)
{
	input_manager->ProcessInput(hwnd, *player);
	player->Update(timeElapsed);
}

LRESULT CSpaceShipScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}
