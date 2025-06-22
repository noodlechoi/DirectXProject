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
	// 정점 버퍼 뷰 설정
	commandList->IASetPrimitiveTopology(primitive_topology);
	commandList->IASetVertexBuffers(slot_num, 1, &vertex_buffer_view);
	if (index_buffer) {
		commandList->IASetIndexBuffer(&index_buffer_view);
		commandList->DrawIndexedInstanced(index_num, 1, start_index, base_vertex_index, 0);
	}
	else {
		// 렌더링(입력 조립기 작동)
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

	// 삼각형 메쉬를 리소스로 생성
	vertex_buffer = CreateBufferResource(device, commandList, vertices, stride * vertex_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, vertex_upload_buffer.GetAddressOf());
	
	// 정점 버퍼 뷰 설정
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

	// 삼각형 메쉬를 리소스로 생성
	vertex_buffer = CreateBufferResource(device, commandList, vertices, stride * vertex_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, vertex_upload_buffer.GetAddressOf());

	// 정점 버퍼 뷰 설정
	vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	vertex_buffer_view.StrideInBytes = stride;
	vertex_buffer_view.SizeInBytes = stride * vertex_num;

	// 인덱스 버퍼 설정
	const size_t intexSize = 36;
	index_num = intexSize;

	UINT indexes[intexSize];

	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	indexes[0] = 3; indexes[1] = 1; indexes[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	indexes[3] = 2; indexes[4] = 1; indexes[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	indexes[6] = 0; indexes[7] = 5; indexes[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	indexes[9] = 1; indexes[10] = 5; indexes[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	indexes[12] = 3; indexes[13] = 4; indexes[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	indexes[15] = 0; indexes[16] = 4; indexes[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	indexes[18] = 1; indexes[19] = 6; indexes[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	indexes[21] = 2; indexes[22] = 6; indexes[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	indexes[24] = 2; indexes[25] = 7; indexes[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	indexes[27] = 3; indexes[28] = 7; indexes[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	indexes[30] = 6; indexes[31] = 4; indexes[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
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
	//위의 그림과 같은 비행기 메쉬를 표현하기 위한 정점 데이터이다.
	CDiffusedVertex vertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy -	y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//비행기 메쉬의 위쪽 면
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
	//비행기 메쉬의 아래쪽 면
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
	//비행기 메쉬의 오른쪽 면
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
	//비행기 메쉬의 뒤쪽/오른쪽 면
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
	//비행기 메쉬의 왼쪽 면
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
	//비행기 메쉬의 뒤쪽/왼쪽 면
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
			//정점의 높이와 색상을 높이 맵으로부터 구한다.
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
				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가한다.
				indices[j++] = (UINT)(x + (z * width));
				indices[j++] = (UINT)((x + (z * width)) + width);
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향이다.
			for (int x = width - 1; x >= 0; x--)
			{
				//줄이 바뀔 때마다(x == (width-1)) 첫 번째 인덱스를 추가한다.
				if (x == (width - 1)) indices[j++] = (UINT)(x + (z * width));
				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가한다.
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
	// 조명 방향 벡터
	XMFLOAT3 lightDirection{ -1.0f, 1.0f, 1.0f };
	lightDirection = Vector3::Normalize(lightDirection);
	CHeightMapImage* heightMapImage = (CHeightMapImage*)context;
	// 조명의 색상(세기, 밝기)
	XMFLOAT4 incidentLightColor{ 0.9f, 0.8f, 0.4f, 1.0f };
	// 조명 색상이 반사되는 비율
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
