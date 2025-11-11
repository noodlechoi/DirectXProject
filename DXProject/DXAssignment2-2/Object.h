#pragma once
#include "Mesh.h"
#include "Texture.h"

class CShader;

// mesh를 가지고 있는 게임 오브젝트 클래스
class CObject
{
public:
	CObject();

	void ReleaseUploadBuffer();

	virtual void SetMesh(CMesh* );
	virtual void SetTexture(CTexture* );
	virtual void Animate(float );
	virtual void Render(ID3D12GraphicsCommandList* );
protected:
	XMFLOAT4X4 world_matrix{};
	std::shared_ptr<CMesh> mesh{};
	std::shared_ptr<CTexture> texture{};
};