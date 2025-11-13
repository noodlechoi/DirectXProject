#include "stdafx.h"
#include "Shader.h"
#include "Object.h"

CObject::CObject()
{
	XMStoreFloat4x4(&world_matrix, XMMatrixIdentity());
}

void CObject::ReleaseUploadBuffer()
{
	// 정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (mesh) {
		mesh->ReleaseUploadBuffer();
	}
}

void CObject::SetMesh(CMesh* otherMesh)
{
	mesh.reset(otherMesh);
}

void CObject::SetTexture(CTexture* otherTexture)
{
	texture.reset(otherTexture);
}

void CObject::Animate(float elapsedTime)
{

}

void CObject::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	UINT elementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255); //256의 배수
	object_cb = CreateBufferResource(device, commandList, NULL, elementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	object_cb->Map(0, NULL, (void**)&mapped_object_info);
}

void CObject::ReleaseShaderVariables()
{
	if (object_cb) object_cb->Unmap(0, NULL);
}

void CObject::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMStoreFloat4x4(&mapped_object_info->world_matrix, XMMatrixTranspose(XMLoadFloat4x4(&world_matrix)));
}

void CObject::Render(ID3D12GraphicsCommandList* commandList)
{
	if (mesh) mesh->Render(commandList);
}