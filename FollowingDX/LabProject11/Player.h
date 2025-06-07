#pragma once
#include "Object.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
public:
	CPlayer(float, float);

	void CreateShaderVariables(ID3D12Device*, ID3D12GraphicsCommandList*) override;

	void Move(const XMFLOAT3& , bool = false);

	void Rotate(float , float , float );

	virtual void Render(ID3D12GraphicsCommandList*);
protected:
	std::unique_ptr<CCamera> camera{};

	XMFLOAT3 position{};
	XMFLOAT3 right{};
	XMFLOAT3 up{};
	XMFLOAT3 look{};

	XMFLOAT3 velocity{};
	XMFLOAT3 gravity{};

	float friction{};

};

class CSpaceShipPlayer : public CPlayer
{
public:
	CSpaceShipPlayer(ID3D12Device*, ID3D12GraphicsCommandList*, float, float);
};