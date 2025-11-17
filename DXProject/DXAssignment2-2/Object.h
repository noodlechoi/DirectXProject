#pragma once
#include "Mesh.h"
#include "Texture.h"

class CShader;
class CCamera;

// mesh를 가지고 있는 게임 오브젝트 클래스
class CObject {
public:
	CObject();

	void ReleaseUploadBuffer();

	void SetMesh(CMesh* );
	void SetTexture(CTexture* );
	CTexture* GetTexture() const { return texture.get(); }
	ID3D12Resource* GetTextureResource() const { return texture->GetTextureResource(); }

	virtual void Animate(float, CCamera*) {};
	virtual void Rotate(float pitch, float yaw, float roll);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* );

	// picking
	void GenerateRayForPicking(XMVECTOR& pickPosition, XMMATRIX& view, XMVECTOR& pickRayOrigin, XMVECTOR& pickRayDirection);
	bool PickObjectByRayIntersection(XMVECTOR& pickPosition, XMMATRIX& view, float* hitDistance);

	//
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 otherPosition) { SetPosition(otherPosition.x, otherPosition.y, otherPosition.z); }
protected:
	XMFLOAT4X4 world_matrix;
	std::shared_ptr<CMesh> mesh{};
	std::shared_ptr<CTexture> texture{};

	bool is_visible{ true };
	BoundingOrientedBox oobb;
};

class CBillboardObject : public CObject {
public:
	void Animate(float, CCamera*) override;
	void SetLookAt(XMFLOAT3& target);
};