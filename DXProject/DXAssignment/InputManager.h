#pragma once

class CScene;
class CObject;

// pointer�� ���� scene�� ���� inputmanager �����
class CInputManager {
public:
	virtual void ProcessInput(HWND&, CScene*) {};
	virtual void ProcessingMouseMessage(HWND&, UINT&, WPARAM&, LPARAM&);
	virtual void ProcessingKeyboardMessage(HWND&, UINT&, WPARAM&, LPARAM&);
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) = 0;
};

class CSpaceShipInputManager : public CInputManager {
public:
	void ProcessInput(HWND&, CScene*) override;
	void ProcessingMouseMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	void ProcessingKeyboardMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
private:
	POINT old_cursor_pos;
};

// ==============
// ��Ŭ�� ���콺 �Է¸�
class CStartInputManager : public CInputManager {
public:
	void ProcessInput(HWND&, CScene*) override;
	void ProcessingMouseMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	void ProcessingKeyboardMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};

class CRollerCoasterInputManager : public CInputManager {
public:
	void ProcessInput(HWND&, CScene*) override;
	void ProcessingMouseMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	void ProcessingKeyboardMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
private:
	POINT old_cursor_pos;
};

class CTankInputManager : public CInputManager {
public:
	void ProcessInput(HWND&, CScene*) override;
	void ProcessingMouseMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	void ProcessingKeyboardMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	DWORD GetTogle() { return togle; }
private:
	DWORD togle{};
	POINT old_cursor_pos;
	bool isFire{};
	CObject* lock_object{};
};