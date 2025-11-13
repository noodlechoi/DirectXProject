#pragma once
#include "Mesh.h"
#include "Texture.h"

class CShader;

struct CB_OBJECT_INFO
{
	XMFLOAT4X4						world_matrix;
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

	void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void ReleaseShaderVariables();
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* );
protected:
	XMFLOAT4X4 world_matrix;
	ComPtr<ID3D12Resource> const_buffer;
	std::unique_ptr<CB_OBJECT_INFO> mapped_object_info;

	std::shared_ptr<CMesh> mesh{};
	std::shared_ptr<CTexture> texture{};
};