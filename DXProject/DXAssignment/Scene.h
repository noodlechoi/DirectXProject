#pragma once
#include "InputManager.h"
#include "Object.h"

class CPlayer;

// ���� �÷��̾�, input�� �����ϴ� Ŭ����
class CScene {
public:
	CScene() {}
	virtual void BuildObjects();
	// �� �Լ������� Object ����
	virtual void CreateObject() {};
	virtual void Animate(float);
	virtual void Render(HDC);
	void PlayerMove(DWORD, float);
	void PlayerRotate(float, float, float);

	virtual void ProcessInput(HWND&, float) {};
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&);

	CObject* PickObjectPointedByCursor(int , int);

	virtual void Save() const;
	virtual void Load();
	virtual CScene* NextScene() { return new CScene; }

	void SetFileName(std::string_view fileName) { file_name = fileName; }
public:
	std::unique_ptr<CInputManager> input_manager{};
	std::unique_ptr<CPlayer> player{};
	// vector�� ������Ʈ ����
	std::deque<std::unique_ptr<CObject>> objects;

	// �̵� ������
	template <typename T, typename Y>
	CScene(size_t, std::unique_ptr<T>&&, std::unique_ptr<Y>&&);
	std::string file_name;
};

class CSpaceShipScene : public CScene {
public:
	CSpaceShipScene();

	void BuildObjects() override;

	void ProcessInput(HWND&, float) override;
	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};

// ==============================
// ����

//// ȸ���ϴ� �̸�
//class CStartScene : public CScene {
//public:
//	CStartScene();
//
//	void BuildObjects() override;
//
//	void ProcessInput(HWND&, float) override;
//	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
//};
//
//class CMenuScene : public CScene {
//public:
//	CMenuScene();
//
//	void BuildObjects() override;
//
//	void ProcessInput(HWND&, float) override;
//	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
//};

class CRollerCoasterScene : public CScene {
public:
	CRollerCoasterScene();

	void CreateObject() override;

	void ProcessInput(HWND&, float) override;
	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
	CScene* NextScene() override;
};

class CTankScene : public CScene  {
public:
	CTankScene();

	void CreateObject() override;
	void CheckObjectByBulletCollisions();
	void Animate(float) override;

	void ProcessInput(HWND&, float) override;
	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};