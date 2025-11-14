#pragma once
#include "Mesh.h"
#include "Texture.h"

class CShader;

struct CB_OBJECT_INFO
{
	XMFLOAT4X4 world_matrix;
};

// mesh를 가지고 있는 게임 오브젝트 클래스
class CObject
{
public:
	CObject();

	void ReleaseUploadBuffer();

	void SetMesh(CMesh* );
	void SetTexture(CTexture* );
	CTexture* GetTexture() const { return texture.get(); }
	ID3D12Resource* GetTextureResource() const { return texture->GetTextureResource(); }

	virtual void Animate(float );

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* );
protected:
	XMFLOAT4X4 world_matrix;
	std::shared_ptr<CMesh> mesh{};
	std::shared_ptr<CTexture> texture{};
};