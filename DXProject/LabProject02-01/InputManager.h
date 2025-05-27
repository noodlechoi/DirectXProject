#pragma once

class CPlayer;

// pointer�� ���� scene�� ���� inputmanager �����
class CInputManager {
public:
	virtual void ProcessInput(HWND&, CPlayer&) {};
	virtual void ProcessingMouseMessage(HWND&, UINT&, WPARAM&, LPARAM&);
	virtual void ProcessingKeyboardMessage(HWND&, UINT&, WPARAM&, LPARAM&);
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) = 0;
};

class CSpaceShipInputManager : public CInputManager {
public:
	void ProcessInput(HWND&, CPlayer&) override;
	void ProcessingMouseMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	void ProcessingKeyboardMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
private:
	POINT old_cursor_pos;
};