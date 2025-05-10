#pragma once
#include "Object.h"

// player 생성자에서 카메라 생성, 상속받은 플레이어에서 메쉬 및 설정
class CPlayer : public CObject {
public:
	CPlayer();
	void SetPosition(float, float, float);

	void LookAt(XMFLOAT3&, XMFLOAT3&);

	virtual void Move(DWORD, float);
	void Move(XMFLOAT3&, bool);
	void Move(float, float, float);

	void Rotate(float = 0.0f, float = 0.0f, float = 0.0f);
	void AroundRotate(float = 0.0f, float = 0.0f, float = 0.0f);


	void SetVelocity(XMFLOAT3& Velocity) { velocity = Velocity; };
	void SetVelocity(XMFLOAT3&& Velocity) { velocity = Velocity; };

	void SetCameraOffset(XMFLOAT3&);
	void SetCameraOffset(XMFLOAT3&&);
	float GetMovingSpeed() const;
	float VelocityToSpeed();

	virtual void Update(float = 0.016f);

	virtual void OnUpdateTransform();
	virtual void Animate(float);
	//virtual void Render(HDC hDCFrameBuffer);

	void SetCamera(std::unique_ptr<CCamera>& pCamera) { camera = std::move(pCamera); }
	std::unique_ptr<CCamera>& GetCamera() { return camera; }
public:
	XMFLOAT3					position{ XMFLOAT3(0.0f, 0.0f, 0.0f) };
	XMFLOAT3					right{ XMFLOAT3(1.0f, 0.0f, 0.0f) };
	XMFLOAT3					up{ XMFLOAT3(0.0f, 1.0f, 0.0f) };
	XMFLOAT3					look{ XMFLOAT3(0.0f, 0.0f, 1.0f) };

	XMFLOAT3					camera_offset{ XMFLOAT3(0.0f, 0.0f, 0.0f) };

	float						friction { 125.0f};

	float           			pitch{};
	float           			yaw{};
	float           			roll{};

	std::unique_ptr<CCamera> camera;
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer();
	void OnUpdateTransform() override;
};

class CNonePlayer : public CPlayer {
public:
	void OnUpdateTransform() override;
};

class CRollerCosterPlayer : public CPlayer {
public:
	CRollerCosterPlayer();
	void OnUpdateTransform() override;
};

class CTankPlayer : public CPlayer {
public:
	CTankPlayer();
	void Update(float = 0.016f) override;
	void OnUpdateTransform() override;
};