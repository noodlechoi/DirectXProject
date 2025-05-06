#pragma once
#include "Mesh.h"

class CCamera;

class CObject
{
public:
	CObject();
	virtual ~CObject();

	CObject(const CObject&);
	CObject& operator=(const CObject&);
	CObject( CObject&&);
	CObject& operator=( CObject&&);

	void SetActive(bool bActive) { is_active = bActive; }
	void SetMesh(CMesh&& );
	void SetMesh(CMesh& );

	void SetColor(DWORD dwColor) { color = dwColor; }

	void SetPosition(float , float , float );
	void SetPosition(XMFLOAT3& );
	void SetPosition(XMFLOAT3&& );

	void SetMovingDirection(XMFLOAT3& xmMovingDirection) { XMStoreFloat3(&moving_direction, XMVector3Normalize(XMLoadFloat3(&moving_direction))); }
	void SetMovingDirection(XMFLOAT3&& xmMovingDirection) { XMStoreFloat3(&moving_direction, XMVector3Normalize(XMLoadFloat3(&moving_direction))); }
	void SetMovingSpeed(float fSpeed) { moving_speed = fSpeed; }
	void SetMovingRange(float fRange) { moving_range = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { XMStoreFloat3(&rotation_axis, XMVector3Normalize(XMLoadFloat3(&xmf3RotationAxis))); }
	void SetRotationAxis(XMFLOAT3&& xmf3RotationAxis) { XMStoreFloat3(&rotation_axis, XMVector3Normalize(XMLoadFloat3(&xmf3RotationAxis))); }
	void SetRotationSpeed(float fSpeed) { rotation_speed = fSpeed; }

	void Move(XMFLOAT3& , float );

	void Rotate(float  = 10.0f, float  = 10.0f, float  = 10.0f);
	void Rotate(XMFLOAT3& , float );

	virtual void OnUpdateTransform() {}

	virtual void Animate(float );
	virtual void Render(HDC );

	// meshes 빼고 저장 및 로드
	virtual void Save(std::ostream&) const;
	virtual std::istream& Load(std::istream&);
protected:
	bool is_active{ true };

	std::vector<CMesh> meshes;
	XMFLOAT4X4 world_matrix;

	DWORD color = RGB(255, 0, 0);	// red

	XMFLOAT3 moving_direction;
	float moving_speed{};
	float moving_range{};

	XMFLOAT3 rotation_axis;
	float rotation_speed{};
};

