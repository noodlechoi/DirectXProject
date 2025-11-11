#pragma once
class CTexture {
public:
	CTexture(std::string& name) : name{ name } {}
	void CreateTextureResource(ID3D12Device* device, ID3D12GraphicsCommandList* commandList ,std::wstring& fileName);

	const char* GetName() const { return name.c_str(); }
private:
	std::string name{};
	ComPtr<ID3D12Resource> texture;
	ComPtr<ID3D12Resource> upload_buffer;
};

