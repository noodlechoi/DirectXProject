#pragma once
#include "InputManager.h"
#include "Object.h"

class CPlayer;

// 씬과 플레이어, input을 관리하는 클래스
class CScene {
public:
	CScene() {}
	virtual void BuildObjects();
	// 이 함수에서만 Object 생성
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
	// vector로 오브젝트 관리
	std::deque<std::unique_ptr<CObject>> objects;

	// 이동 생성자
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
// 과제

//// 회전하는 이름
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