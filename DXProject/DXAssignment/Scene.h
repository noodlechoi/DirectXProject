#pragma once
#include "InputManager.h"
#include "Object.h"

// ���� �÷��̾�, input�� �����ϴ� Ŭ����
class CScene {
public:
	virtual void BuildObjects();
	// �� �Լ������� Object ����
	virtual void CreateObject() {};
	virtual void Animate(float);
	virtual void Render(HDC);

	virtual void ProcessInput(HWND&, float) {};
	virtual LRESULT ProcessingWindowMessage(HWND&, UINT&, WPARAM&, LPARAM&) = 0;

	virtual void Save() const;
	virtual void Load();
protected:
	std::unique_ptr<CInputManager> input_manager{};
	std::unique_ptr<CPlayer> player{};
	// vector�� ������Ʈ ����
	std::vector<CObject> objects;

	// �̵� ������
	template <typename T, typename Y>
	CScene(size_t, std::unique_ptr<T>&&, std::unique_ptr<Y>&&);

	static std::string FileName;
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

// ȸ���ϴ� �̸�
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