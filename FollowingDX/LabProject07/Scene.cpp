#include "stdafx.h"
#include "Scene.h"
CScene::CScene()
{
}

CScene::~CScene()
{
	ReleaseObjects();
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* device)
{
	ID3D12RootSignature* graphicsRootSignature{};

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.NumParameters = 0;
	rootSignatureDesc.pParameters = nullptr;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// 임의 길이 데이터를 반환하는 데 사용
	ComPtr<ID3DBlob> signatureBlob{};
	ComPtr<ID3DBlob> errorBlob{};
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&graphicsRootSignature);

	return graphicsRootSignature;
}

void CScene::ReleaseUploadBuffers()
{
	if (!shaders.empty()) {
		for (int i = 0; i < shaders.size(); ++i) {
			if (shaders[i]) {
				shaders[i]->ReleaseUploadBuffers();
			}
		}
	}
}

void CScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	graphics_root_signature = CreateGraphicsRootSignature(device);

	shaders.push_back(std::make_unique<CShader>());
	shaders[0]->CreateShader(device, graphics_root_signature.Get());
	shaders[0]->BuildObjects(device, commandList);
}

void CScene::ReleaseObjects()
{

}

bool CScene::ProcessInput()
{
	return false;
}

void CScene::AnimateObjects(float elapsedTime)
{
	for (int i = 0; i < shaders.size(); ++i) {
		if (shaders[i]) {
			shaders[i]->AnimateObjects(elapsedTime);
		}
	}
}

void CScene::PrepareRender(ID3D12GraphicsCommandList* commandList)
{
	// Set
	commandList->SetGraphicsRootSignature(graphics_root_signature.Get());
}

void CScene::Render(ID3D12GraphicsCommandList* commandList)
{
	PrepareRender(commandList);

	for (int i = 0; i < shaders.size(); ++i) {
		if (shaders[i]) {
			shaders[i]->Render(commandList);
		}
	}
}

bool CScene::OnProcessMouseMessage(HWND, UINT, WPARAM, LPARAM)
{
	return false;
}

bool CScene::OnProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM)
{
	return false;
}
