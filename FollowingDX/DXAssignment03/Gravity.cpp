#include "stdafx.h"
#include "Gravity.h"

CGravity::CGravity(float otherGravity)
	: gravity{otherGravity}
{
}

void CGravity::Apply(XMFLOAT3& position, XMFLOAT3& velocity, float elapsedTime)
{
	velocity.y += gravity * elapsedTime;
}

void CGravity::ResolveCollision(XMFLOAT3& position, XMFLOAT3& velocity, float groundHeight)
{
    if (position.y <= groundHeight) {
        position.y = groundHeight;
        velocity.y = 0.0f; // ¶¥¿¡ ´ê¾ÒÀ¸¸é ¼Óµµ Á¦°Å
    }
}
