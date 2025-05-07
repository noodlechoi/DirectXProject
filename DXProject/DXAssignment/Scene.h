#pragma once
#include "InputManager.h"
#include "Object.h"

// 씬과 플레이어, input을 관리하는 클래스
class CScene {
public:
	virtual void BuildObjects();
	// 이 함수에서만 Object 생성
	virtual void CreateObject() {};
	virtual void Animate(float);
	virtual void Render(HDC);

	virtual void ProcessInput(HWND&, float) {};
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) = 0;

	virtual void Save() const;
	virtual void Load();

	void SetFileName(std::string_view fileName) { file_name = fileName; }
protected:
	std::unique_ptr<CInputManager> input_manager{};
	std::unique_ptr<CPlayer> player{};
	// vector로 오브젝트 관리
	std::vector<std::unique_ptr<CObject>> objects;

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

// 회전하는 이름
class CStartScene : public CScene {
public:
	CStartScene();

	void BuildObjects() override;

	void ProcessInput(HWND&, float) override;
	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};

class CRollerCoasterScene : public CScene {
public:
	CRollerCoasterScene();

	void CreateObject() override;

	void ProcessInput(HWND&, float) override;
	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};

class CTankScene : CScene {
public:
	CTankScene();

	void CreateObject() override;

	void ProcessInput(HWND&, float) override;
	LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) override;
};