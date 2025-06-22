#include "stdafx.h"
#include "HeightMapImage.h"
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

CMesh::CMesh()
{
}

void CMesh::ReleaseUploadBuffers()
{
	if (vertex_upload_buffer) vertex_upload_buffer.Reset();
}

void CMesh::Render(ID3D12GraphicsCommandList* commandList)
{
	// ���� ���� �� ����
	commandList->IASetPrimitiveTopology(primitive_topology);
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
	CMesh()
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
	CMesh()
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

CAirPlaneMeshDiffused::CAirPlaneMeshDiffused(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, float width, float height, float depth, XMFLOAT4 color)
	: CMesh()
{
	vertex_num = 24 * 3;
	stride = sizeof(CDiffusedVertex);
	float fx = width * 0.5f, fy = height * 0.5f, fz = depth * 0.5f;
	//���� �׸��� ���� ����� �޽��� ǥ���ϱ� ���� ���� �������̴�.
	CDiffusedVertex vertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy -	y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//����� �޽��� ���� ��
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(color,
		RandomColor()));
	//����� �޽��� �Ʒ��� ��
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	//����� �޽��� ������ ��
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(color,
		RandomColor()));
	//����� �޽��� ����/������ ��
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(color,
		RandomColor()));
	//����� �޽��� ���� ��
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(color, RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(color,
		RandomColor()));
	//����� �޽��� ����/���� ��
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(color,
		RandomColor()));
	vertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(color,
		RandomColor()));

	vertex_buffer = ::CreateBufferResource(device, commandList, vertices, stride * vertex_num, D3D12_HEAP_TYPE_DEFAULT,	D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &vertex_upload_buffer);
	vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	vertex_buffer_view.StrideInBytes = stride;
	vertex_buffer_view.SizeInBytes = stride * vertex_num;
}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int xStart, int zStart, int otherWidth, int otherLength, XMFLOAT3 otherScale, XMFLOAT4 color, void* context)
	: CMesh(), width{otherWidth}, length{otherLength}, scale{otherScale}
{
	vertex_num = width * length;
	stride = sizeof(CDiffusedVertex);

	primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	CDiffusedVertex* vertice = new CDiffusedVertex[vertex_num];
	for (int i = 0, z = zStart; z < (zStart + length); z++)
	{
		for (int x = xStart; x < (xStart + width); x++, i++)
		{
			//������ ���̿� ������ ���� �����κ��� ���Ѵ�.
			XMFLOAT3 xmf3Position{ (x * scale.x), OnGetHeight(x, z, context),(z * scale.z) };
			XMFLOAT4 xmf3Color = Vector4::Add(OnGetColor(x, z, context), color);
			vertice[i] = CDiffusedVertex(xmf3Position, xmf3Color);
		}
	}

	vertex_buffer = CreateBufferResource(device, commandList, vertice, stride * vertex_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, vertex_upload_buffer.GetAddressOf());
	vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	vertex_buffer_view.StrideInBytes = stride;
	vertex_buffer_view.SizeInBytes = stride * vertex_num;

	delete[] vertice;


	index_num = ((width * 2) * (length - 1)) + ((length - 1) - 1);
	UINT* indices = new UINT[index_num];

	for (int j = 0, z = 0; z < length - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < width; x++)
			{
				if ((x == 0) && (z > 0)) indices[j++] = (UINT)(x + (z * width));
				//�Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰��Ѵ�.
				indices[j++] = (UINT)(x + (z * width));
				indices[j++] = (UINT)((x + (z * width)) + width);
			}
		}
		else
		{
			//¦�� ��° ���̹Ƿ�(z = 1, 3, 5, ...) �ε����� ���� ������ �����ʿ��� ���� �����̴�.
			for (int x = width - 1; x >= 0; x--)
			{
				//���� �ٲ� ������(x == (width-1)) ù ��° �ε����� �߰��Ѵ�.
				if (x == (width - 1)) indices[j++] = (UINT)(x + (z * width));
				//�Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰��Ѵ�.
				indices[j++] = (UINT)(x + (z * width));
				indices[j++] = (UINT)((x + (z * width)) + width);
			}
		}
	}
	index_buffer = ::CreateBufferResource(device, commandList, indices,	sizeof(UINT) * index_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, index_upload_buffer.GetAddressOf());
	index_buffer_view.BufferLocation = index_buffer->GetGPUVirtualAddress();
	index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
	index_buffer_view.SizeInBytes = sizeof(UINT) * index_num;
	delete[] indices;
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void* context)
{
	CHeightMapImage* heightMapImage = (CHeightMapImage*)context;
	BYTE* heightMapPixels = heightMapImage->GetHeightMapPixels();
	return (heightMapPixels[x + (z * heightMapImage->GetWidth())] * heightMapImage->GetScale().y);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* context)
{
	// ���� ���� ����
	XMFLOAT3 lightDirection{ -1.0f, 1.0f, 1.0f };
	lightDirection = Vector3::Normalize(lightDirection);
	CHeightMapImage* heightMapImage = (CHeightMapImage*)context;
	// ������ ����(����, ���)
	XMFLOAT4 incidentLightColor{ 0.9f, 0.8f, 0.4f, 1.0f };
	// ���� ������ �ݻ�Ǵ� ����
	float mapScale = Vector3::DotProduct(heightMapImage->GetHeightMapNormal(x, z), lightDirection);
	mapScale += Vector3::DotProduct(heightMapImage->GetHeightMapNormal(x + 1, z), lightDirection);
	mapScale += Vector3::DotProduct(heightMapImage->GetHeightMapNormal(x + 1, z + 1), lightDirection);
	mapScale += Vector3::DotProduct(heightMapImage->GetHeightMapNormal(x, z + 1), lightDirection);
	mapScale = (mapScale / 4.0f) + 0.05f;
	if (mapScale > 1.0f) mapScale = 1.0f;
	if(mapScale < 0.25f) mapScale = 0.25f;

	XMFLOAT4 color = Vector4::Multiply(mapScale, incidentLightColor);
	return color;
}
