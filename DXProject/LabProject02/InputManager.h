#pragma once

// pointer로 여러 scene에 따른 inputmanager 만들기
class CInputManager {
public:
	void ProcessingMouseMessage(HWND&, UINT&, WPARAM&, LPARAM&);
	void ProcessingKeyboardMessage(HWND&, UINT&, WPARAM&, LPARAM&);
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) = 0;
};

class CSpaceShipInputManager : CInputManager {
public:
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};