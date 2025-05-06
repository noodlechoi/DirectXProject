#include "stdafx.h"
#include "Scene.h"
#include "Camera.h"
#include "GraphicsPipeline.h"
#include "Player.h"

std::string CRollerCoasterScene::FileName{"data/CRollerCoasterScene.dat"};

template <typename T, typename Y>
CScene::CScene(size_t objectNum, std::unique_ptr<T>&& manager, std::unique_ptr<Y>&& Player)
{
	input_manager.reset(manager.release());
	player.reset(Player.release());
	objects.reserve(objectNum);
}

void CScene::BuildObjects()
{
	try {
		Load();
	}
	catch (const std::runtime_error& e) {
		CreateObject();
		Save();
	}
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

	player->Render(hDCFrameBuffer);

	for (auto& object : objects) {
		object->Render(hDCFrameBuffer);
	}
}


LRESULT CScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}

void CScene::Save() const
{
	std::ofstream out{ FileName.data(), std::ios::binary | std::ios::app };
	for (auto& object : objects) {
		object->Save(out);
	}
	OutputDebugString(L"Save\n");
}

void CScene::Load()
{
	std::ifstream in{ FileName.data(),  std::ios::binary };
	if (!in) {
		throw std::runtime_error("File not found");
	}
	CObject object;
	while (object.Load(in)) {
		object.SetMesh(CCubeMesh());
		objects.push_back(std::make_unique<CObject>(object));
	}
	OutputDebugString(L"Load\n");
	
}

CSpaceShipScene::CSpaceShipScene() : CScene(5, std::make_unique<CSpaceShipInputManager>(), std::make_unique<CAirplanePlayer>())
{
}

void CSpaceShipScene::BuildObjects()
{
	CCubeMesh cube{ 4.0f, 4.0f, 4.0f };
	objects.push_back(std::make_unique<CObject>());
	objects[0]->SetMesh(cube);
	objects[0]->SetColor(RGB(255, 0, 0));
	objects[0]->SetPosition(-13.5f, 0.0f, +14.0f);
	//objects[0]->SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	objects[0]->SetRotationSpeed(90.0f);
	objects[0]->SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
	objects[0]->SetMovingSpeed(0.0f);

	objects.push_back(std::make_unique<CObject>());
	objects[1]->SetMesh(cube);
	objects[1]->SetColor(RGB(0, 0, 255));
	objects[1]->SetPosition(+13.5f, 0.0f, +14.0f);
	//objects[1]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	objects[1]->SetRotationSpeed(180.0f);
	objects[1]->SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	objects[1]->SetMovingSpeed(0.0f);

	objects.push_back(std::make_unique<CObject>());
	objects[2]->SetMesh(cube);
	objects[2]->SetColor(RGB(0, 255, 0));
	objects[2]->SetPosition(0.0f, +5.0f, 20.0f);
	//objects[2]->SetRotationAxis(XMFLOAT3(1.0f, 0.0f, 1.0f));
	objects[2]->SetRotationSpeed(30.15f);
	objects[2]->SetMovingDirection(XMFLOAT3(1.0f, -1.0f, 0.0f));
	objects[2]->SetMovingSpeed(0.0f);

	objects.push_back(std::make_unique<CObject>());
	objects[3]->SetMesh(cube);
	objects[3]->SetColor(RGB(0, 255, 255));
	objects[3]->SetPosition(0.0f, 0.0f, 40.0f);
	//objects[3]->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
	objects[3]->SetRotationSpeed(40.6f);
	objects[3]->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));
	objects[3]->SetMovingSpeed(0.0f);

	objects.push_back(std::make_unique<CObject>());
	objects[4]->SetMesh(cube);
	objects[4]->SetColor(RGB(128, 0, 255));
	objects[4]->SetPosition(10.0f, 10.0f, 50.0f);
	//objects[4]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	objects[4]->SetRotationSpeed(50.06f);
	objects[4]->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 1.0f));
	objects[4]->SetMovingSpeed(0.0f);

}



void CSpaceShipScene::ProcessInput(HWND& hwnd, float timeElapsed)
{
	input_manager->ProcessInput(hwnd, player);
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
	objects[0]->SetActive(true);
	objects[0]->SetMesh(CTextMesh());
	objects[0]->SetColor(RGB(255, 0, 0));
	//objects[0]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	objects[0]->SetRotationSpeed(30.0f);
	objects[0]->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 1.0f));
	objects[0]->SetMovingSpeed(0.0f);
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

void CRollerCoasterScene::CreateObject()
{
	for (int i = 0; i < 5; ++i) {
		CRollerCoaster object;
		CCubeMesh cube{ 4.0f, 4.0f, 4.0f };

		object.SetMesh(cube);
		object.SetColor(RGB(255, 0, 0));
		object.SetPosition(XMFLOAT3(player->position.x , player->position.y - 4.0f, player->position.z - (4.0f * i)));
		object.SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
		object.SetRotationAxis(XMFLOAT3(1.0f, 0.0f, 0.0f));
		object.SetMovingSpeed(5.0f);

		objects.push_back(std::make_unique<CRollerCoaster>(object));
	}
}

void CRollerCoasterScene::Load()
{
	std::ifstream in{ FileName.data(),  std::ios::binary };
	if (!in) {
		throw std::runtime_error("File not found");
	}
	CRollerCoaster object;
	while (object.Load(in)) {
		object.SetMesh(CCubeMesh());
		objects.push_back(std::make_unique<CRollerCoaster>(object));
	}
	OutputDebugString(L"Load\n");
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
