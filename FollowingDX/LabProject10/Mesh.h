#pragma once
class CVertex {
public:
	CVertex() : position{ XMFLOAT3(0.0f, 0.0f, 0.0f)} {}
	CVertex(float x, float y, float z) : position { XMFLOAT3(x, y, z) } {}
	CVertex(XMFLOAT3 Position) : position{Position} {}

	CVertex(const CVertex&);
	CVertex& operator=(const CVertex&);
	CVertex(CVertex&&);
	CVertex& operator=(CVertex&&);
protected:
	XMFLOAT3 position{};
};

class CDiffusedVertex : public CVertex
{
public:
	CDiffusedVertex() : CVertex(), diffuse{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) } {}
	CDiffusedVertex(XMFLOAT3 Position, XMFLOAT4 Diffuse) : CVertex(Position), diffuse(Diffuse) {}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 Diffuse) : CVertex(x, y, z), diffuse{Diffuse} {}
protected:
	// ������ ����
	XMFLOAT4 diffuse;
};

class CMesh
{
public:
	CMesh(ID3D12Device*, ID3D12GraphicsCommandList*);
	virtual ~CMesh() {}

	void AddRef() { ref_num++; }
	void Release() { if (--ref_num <= 0) delete this; }

	void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList*);
private:
	size_t ref_num{};
protected:
	// ���� ����
	ComPtr<ID3D12Resource> vertex_buffer{};
	ComPtr<ID3D12Resource> vertex_upload_buffer{};

	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};

	UINT vertex_num{};
	// �ε��� ����
	ComPtr<ID3D12Resource> index_buffer{};
	ComPtr<ID3D12Resource> index_upload_buffer{};
	D3D12_INDEX_BUFFER_VIEW index_buffer_view{};

	UINT index_num{};
	UINT start_index{};	// �ε��� ���� ���� �ε���
	int base_vertex_index{}; // �ε��� ������ �ε����� ������ �ε���

	D3D12_PRIMITIVE_TOPOLOGY primitive_topology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT slot_num{};
	UINT stride{};
	UINT offset{};
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device*, ID3D12GraphicsCommandList*);
};

class CCubeMeshDiffused : public CMesh
{
public:
	CCubeMeshDiffused(ID3D12Device*, ID3D12GraphicsCommandList*, float = 2.0f, float = 2.0f, float = 2.0f);
};