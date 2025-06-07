#pragma once
#include "Mesh.h"
#include "Shader.h"

// mesh�� ������ �ִ� ���� ������Ʈ Ŭ����
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
	virtual void SetShader(std::shared_ptr<CShader>&);
	
	XMFLOAT3 GetPosition() const { return XMFLOAT3(world_matrix._41, world_matrix._42, world_matrix._43); }
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetRight() const;

	void SetPosition(XMFLOAT3 otherPosition) { SetPosition(otherPosition.x, otherPosition.y, otherPosition.z); }
	void SetPosition(float , float , float );
	
	void Rotate(XMFLOAT3* , float );
	void Rotate(float, float, float);
	
	void Move(float);

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