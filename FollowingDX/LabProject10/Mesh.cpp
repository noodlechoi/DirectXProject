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
	if (index_buffer) {
		commandList->IASetIndexBuffer(&index_buffer_view);
		commandList->DrawIndexedInstanced(index_num, 1, start_index, base_vertex_index, 0);
	}
	else {
		// ������(�Է� ������ �۵�)
		commandList->DrawInstanced(vertex_num, 1, offset, 0);
	}
}

CTriangleMesh::CTriangleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) : 
	CMesh(device, commandList)
{
	vertex_num = 3;
	stride = sizeof(CDiffusedVertex);

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

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, float width, float height, float depth) :
	CMesh(device, commandList)
{
	const size_t vertexSize = 8;

	vertex_num = vertexSize;
	stride = sizeof(CDiffusedVertex);
	float x = width * 0.5f, y = height * 0.5f, z = depth * 0.5f;

	CDiffusedVertex vertices[vertexSize];
	vertices[0] = CDiffusedVertex(XMFLOAT3(-x, +y, -z), RandomColor());
	vertices[1] = CDiffusedVertex(XMFLOAT3(+x, +y, -z), RandomColor());
	vertices[2] = CDiffusedVertex(XMFLOAT3(+x, +y, +z), RandomColor());
	vertices[3] = CDiffusedVertex(XMFLOAT3(-x, +y, +z), RandomColor());
	vertices[4] = CDiffusedVertex(XMFLOAT3(-x, -y, -z), RandomColor());
	vertices[5] = CDiffusedVertex(XMFLOAT3(+x, -y, -z), RandomColor());
	vertices[6] = CDiffusedVertex(XMFLOAT3(+x, -y, +z), RandomColor());
	vertices[7] = CDiffusedVertex(XMFLOAT3(-x, -y, +z), RandomColor());

	// �ﰢ�� �޽��� ���ҽ��� ����
	vertex_buffer = CreateBufferResource(device, commandList, vertices, stride * vertex_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, vertex_upload_buffer.GetAddressOf());

	// ���� ���� �� ����
	vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	vertex_buffer_view.StrideInBytes = stride;
	vertex_buffer_view.SizeInBytes = stride * vertex_num;

	// �ε��� ���� ����
	const size_t intexSize = 36;
	index_num = intexSize;

	UINT indexes[intexSize];

	//�� �ո�(Front) �簢���� ���� �ﰢ��
	indexes[0] = 3; indexes[1] = 1; indexes[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	indexes[3] = 2; indexes[4] = 1; indexes[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	indexes[6] = 0; indexes[7] = 5; indexes[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	indexes[9] = 1; indexes[10] = 5; indexes[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	indexes[12] = 3; indexes[13] = 4; indexes[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	indexes[15] = 0; indexes[16] = 4; indexes[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	indexes[18] = 1; indexes[19] = 6; indexes[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	indexes[21] = 2; indexes[22] = 6; indexes[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	indexes[24] = 2; indexes[25] = 7; indexes[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	indexes[27] = 3; indexes[28] = 7; indexes[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	indexes[30] = 6; indexes[31] = 4; indexes[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	indexes[33] = 7; indexes[34] = 4; indexes[35] = 6;

	index_buffer = CreateBufferResource(device, commandList, indexes, sizeof(UINT) * index_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, vertex_upload_buffer.GetAddressOf());

	index_buffer_view.BufferLocation = index_buffer->GetGPUVirtualAddress();
	index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
	index_buffer_view.SizeInBytes = sizeof(UINT) * index_num;
}
