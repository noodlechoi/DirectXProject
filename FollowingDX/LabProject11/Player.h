#pragma once
#include "Object.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
public:
	CPlayer(CCamera*, float, float);

	void CreateShaderVariables(ID3D12Device*, ID3D12GraphicsCommandList*) override;

	void Move(const XMFLOAT3& , bool = false);
	virtual void Move(DWORD, float);

	void Rotate(float , float , float );
	virtual void Update(float = 0.016f);
	virtual void OnUpdateTransform();

	virtual void Animate(float);
	virtual void Render(ID3D12GraphicsCommandList*);
public:
	std::unique_ptr<CCamera> camera{};

	XMFLOAT3 position{ XMFLOAT3(0.0f, 0.0f, 0.0f) };
	XMFLOAT3 right{ XMFLOAT3(1.0f, 0.0f, 0.0f) };
	XMFLOAT3 up{ XMFLOAT3(0.0f, 1.0f, 0.0f) };
	XMFLOAT3 look{ XMFLOAT3(0.0f, 0.0f, 1.0f) };

	XMFLOAT3 velocity{};
	XMFLOAT3 gravity{};

	float friction{ 125.0f };

};

class CSpaceShipPlayer : public CPlayer
{
public:
	CSpaceShipPlayer(ID3D12Device*, ID3D12GraphicsCommandList*, float, float);
};