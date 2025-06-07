#pragma once
#include "Mesh.h"

class CShader;

// mesh�� ������ �ִ� ���� ������Ʈ Ŭ����
class CGameObject
{
public:
	CGameObject();
	~CGameObject();

	void AddRef() { ref_num++; }
	void Release() { if (--ref_num <= 0) delete this; }

	void ReleaseUploadBuffer();

	virtual void SetMesh(CMesh* );
	virtual void SetShader(CShader* );
	virtual void Animate(float );
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* );
private:
	size_t ref_num{};
protected:
	XMFLOAT4X4 world_matrix{};
	std::shared_ptr<CMesh> mesh{};
};

