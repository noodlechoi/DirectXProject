#pragma once
#include "Mesh.h"

class CCamera;

class CObject
{
public:
	enum class eTYPE {
		Obstacle,
		RollerCoaster,
		Tank,
		Bullet,
	};
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

	void SetMovingDirection(XMFLOAT3& xmMovingDirection) { XMStoreFloat3(&moving_direction, XMVector3Normalize(XMLoadFloat3(&xmMovingDirection)));	}
	void SetMovingDirection(XMFLOAT3&& xmMovingDirection) { XMStoreFloat3(&moving_direction, XMVector3Normalize(XMLoadFloat3(&xmMovingDirection))); }
	virtual void SetMovingSpeed(float fSpeed) { moving_speed = fSpeed; }
	void SetMovingRange(float fRange) { moving_range = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { XMStoreFloat3(&rotation_axis, XMVector3Normalize(XMLoadFloat3(&xmf3RotationAxis))); }
	void SetRotationAxis(XMFLOAT3&& xmf3RotationAxis) { XMStoreFloat3(&rotation_axis, XMVector3Normalize(XMLoadFloat3(&xmf3RotationAxis))); }
	void SetRotationSpeed(float fSpeed) { rotation_speed = fSpeed; }

	void SetType(int Type) { type = Type; }
	void SetType(eTYPE Type) { type = (int)Type; }
	bool EqualType(eTYPE Type) { return type == (int)Type; }


	int GetType() { return type; }
	bool GetActive() { return is_active; }
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	virtual void Move(XMFLOAT3& , float );

	void Rotate(float  = 10.0f, float  = 10.0f, float  = 10.0f);
	void Rotate(XMFLOAT3& , float );

	virtual void OnUpdateTransform() {}

	virtual void Animate(float );
	virtual void Render(HDC );

	// meshes 빼고 저장 및 로드
	virtual void Save(std::ostream&) const;
	virtual std::istream& Load(std::istream&);

	void UpdateBoundingBox();
	void GenerateRayForPicking(XMVECTOR& , XMMATRIX& , XMVECTOR& , XMVECTOR& );
	int PickObjectByRayIntersection(XMVECTOR& , XMMATRIX& , float* );

	BoundingOrientedBox	OOBB{ BoundingOrientedBox() };
protected:
	bool is_active{ true };

	std::vector<CMesh> meshes;
	XMFLOAT4X4 world_matrix;

	DWORD color = RGB(255, 0, 0);	// red

	XMFLOAT3 moving_direction;
	float moving_speed{};
	float moving_range{};
	XMFLOAT3 velocity{ XMFLOAT3(0.0f, 0.0f, 0.0f) };

	XMFLOAT3 rotation_axis;
	float rotation_speed{};

	int type{};
};

class CRollerCoaster : public CObject {
public:
	CRollerCoaster();

	size_t GetPathNum() const { return path.size(); }

	// 업데이트 (경로를 따라 이동)
	void Animate(float) override;
private:
	std::vector<XMFLOAT3> path; // 롤러코스터 경로
	size_t current_index{ 0 };   // 현재 경로 인덱스
};

class CBulletObject : public CObject
{
public:
	CBulletObject(float = 50.0f );
	virtual ~CBulletObject();

	virtual void Animate(float);

	void SetFirePosition(XMFLOAT3 );
	void SetWorldMatrix(XMFLOAT4X4 worldMatrix) { world_matrix = worldMatrix; }
	void Reset();
public:
	float						bullet_effective_range{ 50.0f };
	float						moving_distance{};
	float						rotation_angle{};
	XMFLOAT3					fire_position{ XMFLOAT3(0.0f, 0.0f, 1.0f) };

	float						elapsed_time_after_fire{};
	float						lock_delay_time{0.3f};
	float						locking_time{4.0f};
	CObject* locked_object{};
};

class CEnemyTank : public CObject {
public:
	CEnemyTank();
	void Animate(float)override;
	void Render(HDC)override;

	void FireBullet(CObject* = nullptr);
	void SetNextDestination(XMFLOAT3);

	std::list<CBulletObject> bullets;

private:
	XMFLOAT3 current_distination{};
	XMFLOAT3 next_destination{};
};

//#define EXPLOSION_DEBRISES		240
//class CExplosiveObject : public CObject
//{
//public:
//	CExplosiveObject();
//
//	bool						m_bBlowingUp{ false };
//
//	std::array<XMFLOAT4X4, EXPLOSION_DEBRISES> transform_matrix;
//
//	float						m_fElapsedTimes = 0.0f;
//	float						m_fDuration = 2.0f;
//	float						m_fExplosionSpeed = 10.0f;
//	float						m_fExplosionRotation = 720.0f;
//
//	void Animate(float ) override;
//	void Render(HDC )override;
//
//public:
//	static CMesh* m_pExplosionMesh;
//	static XMFLOAT3	m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
//
//	static void PrepareExplosion();
//};