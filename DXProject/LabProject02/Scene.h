#pragma once
#include "InputManager.h"

class CScene {
public:
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) = 0;
protected:
	std::unique_ptr<CInputManager> input_manager{};

	// 이동 생성자
	template <typename T>
	CScene(std::unique_ptr<T>&&);
};

class CSpaceShipScene : public CScene {
public:
	CSpaceShipScene();

	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};