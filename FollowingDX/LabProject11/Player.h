#pragma once
#include "Camera.h"

class CPlayer
{
public:
	CPlayer(float, float);
	virtual void Render(ID3D12GraphicsCommandList*);
protected:
	std::unique_ptr<CCamera> camera{};
};

