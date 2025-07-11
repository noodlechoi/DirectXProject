#pragma once
#include "Mesh.h"
#include "HeightMapImage.h"
#include "Gravity.h"

// mesh를 가지고 있는 게임 오브젝트 클래스
class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	void AddRef() { ref_num++; }
	void Release() { if (--ref_num <= 0) delete this; }

	void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(ID3D12Device* , ID3D12GraphicsCommandList* );
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* );
	virtual void ReleaseShaderVariables();

	virtual void SetMesh(std::shared_ptr<CMesh>& );
	
	XMFLOAT3 GetPosition() const { return XMFLOAT3(world_matrix._41, world_matrix._42, world_matrix._43); }
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetRight() const;

	void SetPosition(XMFLOAT3 otherPosition) { SetPosition(otherPosition.x, otherPosition.y, otherPosition.z); }
	void SetPosition(float , float , float );
	void SetMovingDirection(XMFLOAT3& xmMovingDirection) { XMStoreFloat3(&moving_direction, XMVector3Normalize(XMLoadFloat3(&xmMovingDirection))); }
	virtual void SetMovingSpeed(float fSpeed) { moving_speed = fSpeed; }

	void Rotate(XMFLOAT3* , float );
	void Rotate(float, float, float);
	
	void Move(float);
	virtual void Move(XMFLOAT3&, float);

	virtual void Animate(float );
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* );

	BoundingOrientedBox	OOBB{ BoundingOrientedBox() };

private:
	size_t ref_num{};
protected:
	XMFLOAT4X4 world_matrix{};
	std::vector<std::shared_ptr<CMesh>> meshes{};

	XMFLOAT3 moving_direction{};
	float moving_speed{};
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject() {};

	void SetRotationAxis(XMFLOAT3 axis) { rotation_axis = axis; }
	void SetRotationSpeed(float speed) { rotation_speed = speed; }

	void Animate(float) override;
private:
	XMFLOAT3 rotation_axis{ 0.0f, 1.0f, 0.0f };
	float rotation_speed{ 15.0f };
};

class CRollerCoaster : public CGameObject {
public:
	CRollerCoaster();

	size_t GetPathNum() const { return path.size(); }

	// 업데이트 (경로를 따라 이동)
	void Animate(float) override;
private:
	std::vector<XMFLOAT3> path; // 롤러코스터 경로
	size_t current_index{ 0 };   // 현재 경로 인덱스
};

class CBulletObject : public CGameObject
{
public:
	CBulletObject(float = 50.0f);
	virtual ~CBulletObject();

	void Animate(float) override;

	void SetFirePosition(XMFLOAT3);
	void SetWorldMatrix(XMFLOAT4X4 worldMatrix) { world_matrix = worldMatrix; }
	void Reset();
public:
	float						bullet_effective_range{ 50.0f };
	float						moving_distance{};
	float						rotation_angle{};
	XMFLOAT3					fire_position{ XMFLOAT3(0.0f, 0.0f, 1.0f) };

	float						elapsed_time_after_fire{};
	float						lock_delay_time{ 0.3f };
	float						locking_time{ 4.0f };
	CGameObject* locked_object{};
};

class CEnemyTank : public CGameObject {
public:
	CEnemyTank();

	void Animate(float)override;
	void FireBullet(CGameObject* = nullptr);
	void SetNextDestination(XMFLOAT3);
private:
	//CHeightMapTerrain* terrain;
	CGravity gravity;
	XMFLOAT3 current_distination{};
	XMFLOAT3 next_destination{};
};

class CHeightMapTerrain : public CGameObject {
public:
	CHeightMapTerrain(ID3D12Device*, ID3D12GraphicsCommandList*, LPCTSTR, int, int, int, int, XMFLOAT3, XMFLOAT4);

	float GetHeight(float x, float z) { return(height_map_image->GetHeight(x / scale.x, z / scale.z) * scale.y); };
	XMFLOAT3 GetNormal(float x, float z) { return(height_map_image->GetHeightMapNormal(int(x / scale.x),int( z / scale.z))); };

	int GetHeightMapWidth() { return height_map_image->GetWidth(); }
	int GetHeightMapLength() { return height_map_image->GetLength(); }
	XMFLOAT3 GetScale() { return scale; }

	float GetWidth() { return width * scale.x; }
	float GetLength() { return length* scale.z; }
private:
	std::unique_ptr<CHeightMapImage> height_map_image;

	int width, length;

	XMFLOAT3 scale;
};