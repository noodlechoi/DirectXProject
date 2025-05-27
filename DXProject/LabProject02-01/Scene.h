#pragma once
#include "InputManager.h"
#include "Object.h"

class CScene {
public:
	virtual void BuildObjects() {}
	virtual void Animate(float);
	virtual void Render(HDC);
	virtual void ProcessInput(HWND&, float) {};

	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) = 0;
protected:
	std::unique_ptr<CInputManager> input_manager{};
	std::unique_ptr<CPlayer> player{};
	std::deque<std::unique_ptr<CObject>> objects;

	template <typename T, typename Y>
	CScene(size_t, T&&,Y&&);
};

class CSpaceShipScene : public CScene {
public:
	CSpaceShipScene();

	void BuildObjects() override;

	void ProcessInput(HWND&, float) override;
	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
private:
	std::array<CObject, 5> objects;
};