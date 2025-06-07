#pragma once
#include "Mesh.h"
#include "Shader.h"

// mesh를 가지고 있는 게임 오브젝트 클래스
class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	void AddRef() { ref_num++; }
	void Release() { if (--ref_num <= 0) delete this; }

	void ReleaseUploadBuffers();

	virtual void SetMesh(CMesh* );
	virtual void SetShader(CShader* );
	
	void Rotate(XMFLOAT3* , float );
	
	virtual void Animate(float );
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* );
private:
	size_t ref_num{};
protected:
	XMFLOAT4X4 world_matrix{};
	std::shared_ptr<CMesh> mesh{};
	std::shared_ptr<CShader> shader{};
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
	float rotation_speed{ 90.0f };
};