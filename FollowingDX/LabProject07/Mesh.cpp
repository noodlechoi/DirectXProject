#include "stdafx.h"
#include "Mesh.h"

CVertex::CVertex(const CVertex& rhs) : position{ rhs.position }
{

}

CVertex& CVertex::operator=(const CVertex& rhs)
{
	if (this == &rhs) return *this;
	position = rhs.position;

	return *this;
}

CVertex::CVertex(CVertex&& other) : position{ std::move(other.position) }
{
}

CVertex& CVertex::operator=(CVertex&& other)
{
	if (this == &other) return *this;
	position = std::move(other.position);

	return *this;
}

CMesh::CMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
}

void CMesh::ReleaseUploadBuffers()
{
	if (vertex_upload_buffer) vertex_upload_buffer.Reset();
}

void CMesh::Render(ID3D12GraphicsCommandList* commandList)
{
	// ���� ���� �� ����
	commandList->IASetVertexBuffers(slot_num, 1, &vertex_buffer_view);
	// ������(�Է� ������ �۵�)
	commandList->DrawInstanced(vertex_num, 1, offset, 0);
}

CTriangleMesh::CTriangleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) : 
	CMesh(device, commandList)
{
	vertex_num = 3;
	stride = sizeof(CDiffusedVertex);
	primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex vertices[] = {
		CDiffusedVertex(XMFLOAT3(0.0f, 0.5, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)),
		CDiffusedVertex(XMFLOAT3(0.5f, -0.5, 0.0f), XMFLOAT4(0.0f,1.0f, 0.0f, 1.0f)),
		CDiffusedVertex(XMFLOAT3(-0.5f, -0.5, 0.0f), XMFLOAT4(Colors::Blue))
	};

	// �ﰢ�� �޽��� ���ҽ��� ����
	vertex_buffer = CreateBufferResource(device, commandList, vertices, stride * vertex_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, vertex_upload_buffer.GetAddressOf());
	
	// ���� ���� �� ����
	vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	vertex_buffer_view.StrideInBytes = stride;
	vertex_buffer_view.SizeInBytes = stride * vertex_num;
}
