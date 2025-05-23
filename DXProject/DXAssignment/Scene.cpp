#include "stdafx.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"
#include "GraphicsPipeline.h"
#include <random>

template <typename T, typename Y>
CScene::CScene(size_t objectNum, std::unique_ptr<T>&& manager, std::unique_ptr<Y>&& Player)
{
	input_manager.reset(manager.release());
	player.reset(Player.release());
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

void CScene::PlayerMove(DWORD direction, float distance)
{
	player->Move(direction, distance);
}

void CScene::PlayerRotate(float pitch, float yaw, float roll)
{
	player->Rotate(pitch, yaw, roll);
}


LRESULT CScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}

CObject* CScene::PickObjectPointedByCursor(int xClient, int yClient)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)player->camera->viewport.width) - 1) / player->camera->perspective_project_matrix._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)player->camera->viewport.height) - 1) / player->camera->perspective_project_matrix._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&player->camera->view_matrix);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CObject* nearestObject = NULL;
	for(std::unique_ptr<CObject>& object : objects) {
		float fHitDistance = FLT_MAX;
		nIntersected = object->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))	{
			fNearestHitDistance = fHitDistance;
			nearestObject = object.get();
		}
	}

	return nearestObject;
}

void CScene::Save() const {
	std::ofstream out{ file_name.data(), std::ios::binary };
	if (!out) {
		throw std::runtime_error("Failed to open file for saving");
	}

	// ��ü �� ����
	int objectCount = static_cast<int>(objects.size());
	out.write(reinterpret_cast<const char*>(&objectCount), sizeof(objectCount));

	// �� ��ü ����
	for (const auto& object : objects) {
		// ��ü Ÿ�� ����
		int type = object->GetType();
		out.write(reinterpret_cast<const char*>(&type), sizeof(int));

		object->Save(out);
	}

	OutputDebugString(L"Save completed\n");
}



void CScene::Load()
{
	std::ifstream in{ file_name.data(), std::ios::binary };
	if (!in) {
		throw std::runtime_error("File not found");
	}

	// ��ü �� �б�
	int objectCount{};
	in.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));
	int type{};
	for (int i = 0; i < objectCount; ++i) {
		// ��ü Ÿ�� load
		in.read(reinterpret_cast<char*>(&type), sizeof(int));
		std::unique_ptr<CObject> object;
		switch (type) {
		case (int)CObject::eTYPE::Obstacle:
			object = std::make_unique<CObject>();
			break;
		case (int)CObject::eTYPE::RollerCoaster:
			object = std::make_unique<CRollerCoaster>();
			break;
		case (int)CObject::eTYPE::Tank:
			object = std::make_unique<CEnemyTank>();
			break;
		default:
			object = std::make_unique<CObject>();
			break;
		}
		object->SetType(type);

		object->Load(in);
		objects.push_back(std::move(object));
	}

	OutputDebugString(L"Load completed\n");
	
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
	input_manager->ProcessInput(hwnd, this);
	player->Update(timeElapsed);
}

LRESULT CSpaceShipScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}

// ==================
//
//CStartScene::CStartScene() : CScene{1, std::make_unique<CStartInputManager>(), std::make_unique<CNonePlayer>() }
//{
//}
//
//void CStartScene::BuildObjects()
//{
//	// ȸ���ϴ� �̸�
//	CObject object;
//	object.SetActive(true);
//	object.SetMesh(CTextMesh());
//	object.SetColor(RGB(255, 0, 0));
//	//object.->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
//	object.SetRotationSpeed(30.0f);
//	object.SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 1.0f));
//	object.SetMovingSpeed(0.0f);
//	objects.emplace_back(object);
//}
//
//
//void CStartScene::ProcessInput(HWND& hwnd, float timeElapsed)
//{
//	input_manager->ProcessInput(hwnd, this);
//	//player->Update(timeElapsed);
//}
//
//LRESULT CStartScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
//{
//	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
//}
//
//
//CMenuScene::CMenuScene() : CScene{ 1, std::make_unique<CStartInputManager>(), std::make_unique<CNonePlayer>() }
//{
//}
//
//void CMenuScene::BuildObjects()
//{
//}
//
//void CMenuScene::ProcessInput(HWND&, float)
//{
//}
//
//LRESULT CMenuScene::ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&)
//{
//	return LRESULT();
//}


// ==================
CRollerCoasterScene::CRollerCoasterScene() : CScene{ 10, std::make_unique<CRollerCoasterInputManager>(), std::make_unique<CRollerCosterPlayer>()}
{
	SetFileName("data/CRollerCoasterScene.dat");
}

void CRollerCoasterScene::CreateObject()
{
	CCubeMesh cube{ 4.0f, 4.0f, 4.0f };
	for (int i = 0; i < 5; ++i) {
		CRollerCoaster object;

		object.SetMesh(cube);
		object.SetColor(RGB(255, 0, 0));
		object.SetPosition(XMFLOAT3(player->position.x , player->position.y - 4.0f, player->position.z - (6.0f * i)));
		object.SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
		object.SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 0.0f));
		object.SetMovingSpeed(5.0f);

		objects.push_back(std::make_unique<CRollerCoaster>(object));
	}
	CObject object;
	CCubeMesh bigCube{ 10.0f, 10.0f, 10.0f };
	object.SetMesh(bigCube);
	object.SetColor(RGB(0, 0, 0));
	object.SetPosition(XMFLOAT3(0.0f, 0.0f, 50.0f));
	object.SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	object.SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 0.0f));
	object.SetMovingSpeed(0.0f);

	objects.push_back(std::make_unique<CObject>(object));
}

void CRollerCoasterScene::ProcessInput(HWND& hwnd, float timeElapsed)
{
	input_manager->ProcessInput(hwnd, this);
	XMFLOAT3 objectPosition = objects[0]->GetPosition();
	player->position = XMFLOAT3(objectPosition.x, objectPosition.y + 4.0f, objectPosition.z);
	player->Update(timeElapsed);
}

LRESULT CRollerCoasterScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}

CScene* CRollerCoasterScene::NextScene()
{
	return new CTankScene();
}

CTankScene::CTankScene() : CScene(15, std::make_unique<CTankInputManager>(), std::make_unique<CTankPlayer>())
{
	SetFileName("data/CTankScene.data");
}

void CTankScene::CreateObject()
{
	//{
	//	// �ٴ�
	//	CCubeMesh cube{ 50.0f, 2.0f, 50.0f };
	//	CObject object;
	//	object.SetMesh(cube);
	//	object.SetColor(RGB(0, 0, 0));
	//	object.SetPosition(XMFLOAT3(0.0f, -2.0f, 0.0f));
	//	object.SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	//	object.SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 0.0f));
	//	object.SetMovingSpeed(0.0f);

	//	objects.push_back(std::make_unique<CObject>(object));
	//}

	// ��ũ
	for (int i = 0; i < 15; ++i) {
		CEnemyTank object;

		objects.emplace_back(std::make_unique<CEnemyTank>(object));
	}
}

void CTankScene::CheckObjectByBulletCollisions()
{
	std::list<CBulletObject>& copyBullets = (dynamic_cast<CTankPlayer*>(player.get()))->bullets;
	for (auto bulletIt = copyBullets.begin(); bulletIt != copyBullets.end();) {
		bool isErase{ false };
		for (auto objectIt = objects.begin(); objectIt != objects.end(); ++objectIt) {
			if (objectIt->get()->OOBB.Intersects(bulletIt->OOBB)) {
				isErase = true;
				objects.erase(objectIt);
				break;
			}
		}
		if (isErase) {
			bulletIt = copyBullets.erase(bulletIt);
		}
		else {
			++bulletIt;
		}
	}
}

void CTankScene::Animate(float elapsedTime)
{
	CScene::Animate(elapsedTime);
	CheckObjectByBulletCollisions();
}

void CTankScene::ProcessInput(HWND& hwnd, float timeElapsed)
{
	input_manager->ProcessInput(hwnd, this);
	player->Update(timeElapsed);

}

LRESULT CTankScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}
