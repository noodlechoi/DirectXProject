#include "stdafx.h"
#include "Scene.h"

template <typename T>
CScene::CScene(std::unique_ptr<T>&& manager)
{
	input_manager.reset(manager.release());
}

LRESULT CScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}

CSpaceShipScene::CSpaceShipScene() : CScene(std::make_unique<CSpaceShipInputManager>())
{
}

LRESULT CSpaceShipScene::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	return input_manager->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}
