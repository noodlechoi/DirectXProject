#include "stdafx.h"
#include "Texture.h"
#include "DDSTextureLoader.h"

void CTexture::CreateTextureResource(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, std::wstring& fileName)
{
	ThrowIfFailed(CreateDDSTextureFromFile12(device, commandList, fileName.c_str(), texture, upload_buffer));
}
