#include "stdafx.h"
#include "Mesh.h"

// CDiffuseVertex
CDiffuseVertex::CDiffuseVertex()
	: CVertex(), tex{}
{
}

CDiffuseVertex::CDiffuseVertex(XMFLOAT3 position, XMFLOAT4 color, XMFLOAT2 tex)
	: CVertex(position, color), tex{ tex }
{
}

// CMesh
CMesh::CMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
}

void CMesh::ReleaseUploadBuffer()
{
	if (vertex_upload_buffer) vertex_upload_buffer.Reset();
	vertex_upload_buffer = nullptr;
}

void CMesh::Render(ID3D12GraphicsCommandList* commandList)
{
	// 프리미티브 유형 설정
	commandList->IASetPrimitiveTopology(primitive_topology);
	// 정점 버퍼 뷰 설정
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


// CTriangleMesh
CTriangleMesh::CTriangleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
	: CMesh(device, commandList)
{
	vertex_num = 3;
	stride = sizeof(CVertex);
	primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CVertex vertices[] = {
		CVertex(XMFLOAT3(0.0f, 0.5, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)),
		CVertex(XMFLOAT3(0.5f, -0.5, 0.0f), XMFLOAT4(0.0f,1.0f, 0.0f, 1.0f)),
		CVertex(XMFLOAT3(-0.5f, -0.5, 0.0f), XMFLOAT4(Colors::Blue))
	};

	// 삼각형 메쉬를 리소스로 생성
	vertex_buffer = CreateBufferResource(device, commandList, vertices, stride * vertex_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, vertex_upload_buffer.GetAddressOf());
	
	// 정점 버퍼 뷰 설정
	vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	vertex_buffer_view.StrideInBytes = stride;
	vertex_buffer_view.SizeInBytes = stride * vertex_num;
}

// CRectangleMesh
CRectangleMesh::CRectangleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
	: CMesh(device, commandList)
{
	// 정점 버퍼 생성
	vertex_num = 4;
	stride = sizeof(CDiffuseVertex);
	primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffuseVertex vertices[] = {
		CDiffuseVertex(XMFLOAT3(-0.5f, 0.5, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)),
		CDiffuseVertex(XMFLOAT3(0.5f, 0.5, 0.0f), XMFLOAT4(0.0f,1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)),
		CDiffuseVertex(XMFLOAT3(0.5f, -0.5, 0.0f), XMFLOAT4(Colors::Blue), XMFLOAT2(1.0f, 1.0f)),
		CDiffuseVertex(XMFLOAT3(-0.5f, -0.5, 0.0f), XMFLOAT4(Colors::Blue), XMFLOAT2(0.0f, 1.0f))
	};

	// 삼각형 메쉬를 리소스로 생성
	vertex_buffer = CreateBufferResource(device, commandList, vertices, stride * vertex_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, vertex_upload_buffer.GetAddressOf());

	// 정점 버퍼 뷰 설정
	vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	vertex_buffer_view.StrideInBytes = stride;
	vertex_buffer_view.SizeInBytes = stride * vertex_num;

	// 인덱스 버퍼 생성
	index_num = 6;
	UINT indexes[] = {
		0,1,3,
		1,2,3
	};

	index_buffer = CreateBufferResource(device, commandList, indexes, sizeof(UINT) * index_num, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, vertex_upload_buffer.GetAddressOf());

	index_buffer_view.BufferLocation = index_buffer->GetGPUVirtualAddress();
	index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
	index_buffer_view.SizeInBytes = sizeof(UINT) * index_num;
}