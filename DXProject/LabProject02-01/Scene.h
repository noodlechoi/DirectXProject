#pragma once
#include "InputManager.h"
#include "Object.h"

class CScene {
public:
	virtual void BuildObjects() {}
	virtual void Animate(float) {};
	virtual void Render(HDC, std::unique_ptr<CCamera>&) {};
	virtual void ProcessInput(HWND&, std::unique_ptr<CPlayer>&) {};

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

	void BuildObjects() override;
	void Animate(float) override;
	void Render(HDC, std::unique_ptr<CCamera>&) override;

	void ProcessInput(HWND&, std::unique_ptr<CPlayer>&);
	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
private:
	std::array<CObject, 5> objects;
};