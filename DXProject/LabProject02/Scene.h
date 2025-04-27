#pragma once
#include "InputManager.h"

class CScene {
public:
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) = 0;
protected:
	std::unique_ptr<CInputManager> input_manager{};
};

class CSpaceShipScene : public CScene {
public:
	CSpaceShipScene();

	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};