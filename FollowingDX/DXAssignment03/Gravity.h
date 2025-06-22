#pragma once
class CGravity
{
public:
	CGravity(float = -9.8f);

	void Apply(XMFLOAT3& , XMFLOAT3& , float );
	void ResolveCollision(XMFLOAT3& , XMFLOAT3& , float );
private:
	float gravity;
};

