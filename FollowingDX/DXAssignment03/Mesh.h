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
	// 정점의 색상
	XMFLOAT4 diffuse;
};

class CMesh
{
public:
	CMesh();
	virtual ~CMesh() {}

	void AddRef() { ref_num++; }
	void Release() { if (--ref_num <= 0) delete this; }

	void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList*);
private:
	size_t ref_num{};
protected:
	// 정점 버퍼
	ComPtr<ID3D12Resource> vertex_buffer{};
	ComPtr<ID3D12Resource> vertex_upload_buffer{};

	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};

	UINT vertex_num{};
	// 인덱스 버퍼
	ComPtr<ID3D12Resource> index_buffer{};
	ComPtr<ID3D12Resource> index_upload_buffer{};
	D3D12_INDEX_BUFFER_VIEW index_buffer_view{};

	UINT index_num{};
	UINT start_index{};	// 인덱스 버퍼 시작 인덱스
	int base_vertex_index{}; // 인덱스 버퍼의 인덱스에 더해질 인덱스

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

class CAirPlaneMeshDiffused : public CMesh
{
public:
	CAirPlaneMeshDiffused(ID3D12Device* , ID3D12GraphicsCommandList* , float  = 20.0f, float  = 20.0f, float  = 4.0f, XMFLOAT4 = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
};

class CTankMesh : public CMesh {
public:
	CTankMesh(ID3D12Device*, ID3D12GraphicsCommandList*);
};

class CHeightMapGridMesh : public CMesh {
public:
	CHeightMapGridMesh(ID3D12Device*, ID3D12GraphicsCommandList*, int, int, int, int, XMFLOAT3 = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* = nullptr);

	XMFLOAT3 GetScale() { return scale; }
	int GetWidth() { return width; }
	int GetLength() { return length; }

	// 격자 좌표 정점 높이 반환
	virtual float OnGetHeight(int, int, void*);
	virtual XMFLOAT4 OnGetColor(int, int, void*);
protected:
	XMFLOAT3 scale;
	int width, length;
};