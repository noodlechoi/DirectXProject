#pragma once

// pointer�� ���� scene�� ���� inputmanager �����
class CInputManager {
public:
	void ProcessingMouseMessage(HWND&, UINT&, WPARAM&, LPARAM&);
	void ProcessingKeyboardMessage(HWND&, UINT&, WPARAM&, LPARAM&);
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) = 0;
};

class CSpaceShipInputManager : public CInputManager {
public:
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};