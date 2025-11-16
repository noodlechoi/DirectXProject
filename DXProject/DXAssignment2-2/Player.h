#pragma once
#include "Object.h"

class CCamera;


class CPlayer : public CObject{
public:
	CPlayer();
	void Rotate(float pitch, float yaw, float roll) override;
protected:
	std::unique_ptr<CCamera> camera;
};

