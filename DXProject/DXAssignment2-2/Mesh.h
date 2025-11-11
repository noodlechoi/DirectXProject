#pragma once
class CVertex {
public:
	CVertex() : position{ XMFLOAT3(0.0f, 0.0f, 0.0f) }, color{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) } {}
	CVertex(XMFLOAT3 position, XMFLOAT4 color) : position{ position }, color{color} {}

protected:
	XMFLOAT3 position{};
	XMFLOAT4 color{};
};

class CMesh
{
public:
	CMesh(ID3D12Device*, ID3D12GraphicsCommandList*);

	void ReleaseUploadBuffer();

	virtual void Render(ID3D12GraphicsCommandList*);
protected:
	ComPtr<ID3D12Resource> vertex_buffer{};
	ComPtr<ID3D12Resource> vertex_upload_buffer{};

	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};

	D3D12_PRIMITIVE_TOPOLOGY primitive_topology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT slot_num{};
	UINT vertex_num{};
	UINT stride{};
	UINT offset{};
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device*, ID3D12GraphicsCommandList*);
};