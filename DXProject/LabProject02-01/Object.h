#pragma once
#include "Mesh.h"

class CCamera;

class CObject
{
public:
	CObject();

	void SetMesh(CMesh);

	void SetColor(DWORD dwColor) { color = dwColor; }

	void SetPosition(float , float , float );
	void SetPosition(XMFLOAT3& );

	void SetMovingDirection(XMFLOAT3& movingDirection) { moving_direction = Vector3::Normalize(movingDirection); }
	void SetMovingSpeed(float Speed) { moving_speed = Speed; }

	void SetRotationAxis(XMFLOAT3& rotationAxis) { rotation_axis = Vector3::Normalize(rotationAxis); }
	void SetRotationSpeed(float Speed) { rotation_speed = Speed; }

	void Move(XMFLOAT3& , float );

	// world_matrix에 rotate 행렬 변환
	void Rotate(float  = 10.0f, float  = 10.0f, float  = 10.0f);
	void Rotate(XMFLOAT3& , float );

	BoundingBox MergeMeshesBoundingBox();
	void UpdateBoundingBox();
	
	// 광선을 모델 좌표계로 변환
	void GenerateRayForPicking(XMVECTOR&, XMMATRIX&, XMVECTOR&, XMVECTOR&);
	// 광선 피킹 체크
	int PickObjectByRayIntersection(XMVECTOR&, XMMATRIX&, float&);

	virtual void OnUpdateTransform() {}

	virtual void Animate(float );
	virtual void Render(HDC , CCamera&);

	BoundingOrientedBox OBB{ BoundingOrientedBox() };
protected:
	std::vector<CMesh> meshes;
	XMFLOAT4X4 world_matrix;

	DWORD color = RGB(255, 0, 0);	// red

	XMFLOAT3 moving_direction;
	float moving_speed{};

	XMFLOAT3 rotation_axis;
	float rotation_speed{};

};

