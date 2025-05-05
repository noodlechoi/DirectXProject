#include "stdafx.h"
#include "Scene.h"
#include "Camera.h"
#include "GraphicsPipeline.h"
#include "Player.h"

template <typename T, typename Y>
CScene::CScene(size_t objectNum, std::unique_ptr<T>&& manager, std::unique_ptr<Y>&& Player)
{
	input_manager.reset(manager.release());
	player.reset(Player.release());
	objects.reserve(objectNum);
}


void CScene::Animate(float elapsedTime)
{
	player->Animate(elapsedTime);

	for (auto& object : objects) {
		object.Animate(elapsedTime);
	}
}

void CScene::Render(HDC hDCFrameBuffer)
{
	CGraphicsPipeline::SetViewport(player->camera->viewport);
	CGraphicsPipeline::SetViewPerspectiveProjectTransform(player->camera->view_perspective_project_matrix);

	player->Render(hDCFrameBuffer);

	for (auto& object : objects) {
		object.Render(hDCFrameBuffer);
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
	objects.push_back(CObject());
	objects[0].SetMesh(cube);
	objects[0].SetColor(RGB(255, 0, 0));
	objects[0].SetPosition(-13.5f, 0.0f, +14.0f);
	objects[0].SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	objects[0].SetRotationSpeed(90.0f);
	objects[0].SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
	objects[0].SetMovingSpeed(0.0f);

	objects.push_back(CObject());
	objects[1].SetMesh(cube);
	objects[1].SetColor(RGB(0, 0, 255));
	objects[1].SetPosition(+13.5f, 0.0f, +14.0f);
	objects[1].SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	objects[1].SetRotationSpeed(180.0f);
	objects[1].SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	objects[1].SetMovingSpeed(0.0f);

	objects.push_back(CObject());
	objects[2].SetMesh(cube);
	objects[2].SetColor(RGB(0, 255, 0));
	objects[2].SetPosition(0.0f, +5.0f, 20.0f);
	objects[2].SetRotationAxis(XMFLOAT3(1.0f, 0.0f, 1.0f));
	objects[2].SetRotationSpeed(30.15f);
	objects[2].SetMovingDirection(XMFLOAT3(1.0f, -1.0f, 0.0f));
	objects[2].SetMovingSpeed(0.0f);

	objects.push_back(CObject());
	objects[3].SetMesh(cube);
	objects[3].SetColor(RGB(0, 255, 255));
	objects[3].SetPosition(0.0f, 0.0f, 40.0f);
	objects[3].SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
	objects[3].SetRotationSpeed(40.6f);
	objects[3].SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));
	objects[3].SetMovingSpeed(0.0f);

	objects.push_back(CObject());
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
	input_manager->ProcessInput(hwnd, player);
	// 인풋 후 업데이트
	player->Update(timeElapsed);
}

LRESULT CSpaceShipScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}

// ==================

CStartScene::CStartScene() : CScene{1, std::make_unique<CStartInputManager>(), std::make_unique<CNonePlayer>() }
{
}

void CStartScene::BuildObjects()
{
	// 회전하는 이름
	CTextMesh textMesh;
	objects[0].SetActive(true);
	objects[0].SetMesh(CTextMesh());
	objects[0].SetColor(RGB(255, 0, 0));
	objects[0].SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	objects[0].SetRotationSpeed(30.0f);
	objects[0].SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 1.0f));
	objects[0].SetMovingSpeed(0.0f);
}


void CStartScene::ProcessInput(HWND& hwnd, float timeElapsed)
{
	input_manager->ProcessInput(hwnd, player);
	player->Update(timeElapsed);
}

LRESULT CStartScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}

// ==================
CRollerCoasterScene::CRollerCoasterScene() : CScene{ 3, std::make_unique<CRollerCoasterInputManager>(), std::make_unique<CRollerCosterPlayer>()}
{
}

void CRollerCoasterScene::BuildObjects()
{
	CObject object;
	CCubeMesh cube{ 4.0f, 4.0f, 4.0f };
	object.SetMesh(cube);
	object.SetColor(RGB(255, 0, 0));
	object.SetPosition(-13.5f, 0.0f, +14.0f);
	object.SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	object.SetRotationSpeed(90.0f);
	object.SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
	object.SetMovingSpeed(0.0f);

	objects.push_back(object);
}


void CRollerCoasterScene::ProcessInput(HWND& hwnd, float timeElapsed)
{
	input_manager->ProcessInput(hwnd, player);
	player->Update(timeElapsed);
}

LRESULT CRollerCoasterScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}
