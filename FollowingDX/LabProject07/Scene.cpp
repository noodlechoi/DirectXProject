#include "stdafx.h"
#include "Shader.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* device)
{
	ID3D12RootSignature* graphicsRootSignature{};

	D3D12_ROOT_PARAMETER rootParameters[2];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[0].Constants.Num32BitValues = 16;
	rootParameters[0].Constants.ShaderRegister = 0;
	rootParameters[0].Constants.RegisterSpace = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[1].Constants.Num32BitValues = 32;
	rootParameters[1].Constants.ShaderRegister = 1;
	rootParameters[1].Constants.RegisterSpace = 0;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.NumParameters = _countof(rootParameters);
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = rootSignatureFlags;

	// 임의 길이 데이터를 반환하는 데 사용
	ComPtr<ID3DBlob> signatureBlob{};
	ComPtr<ID3DBlob> errorBlob{};
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&graphicsRootSignature);

	return graphicsRootSignature;
}

void CScene::ReleaseUploadBuffers()
{
	if (!objects.empty()) {
		for (int i = 0; i < objects.size(); ++i) {
			if (objects[i]) {
				objects[i]->ReleaseUploadBuffers();
			}
		}
	}
}

void CScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	graphics_root_signature = CreateGraphicsRootSignature(device);

	CTriangleMesh* pTriangleMesh = new CTriangleMesh(device, commandList);

	for (int i = 0; i < 1; ++i) {
		objects.push_back(std::make_unique<CRotatingObject>());
	}
	objects[0]->SetMesh(pTriangleMesh);
	
	CDiffusedShader* shader = new CDiffusedShader;
	shader->CreateShader(device, graphics_root_signature.Get());
	shader->CreateShaderVariables(device, commandList);

	objects[0]->SetShader(shader);
}


bool CScene::ProcessInput()
{
	return false;
}

void CScene::AnimateObjects(float elapsedTime)
{
	for (int i = 0; i < objects.size(); ++i) {
		if (objects[i]) {
			objects[i]->Animate(elapsedTime);
		}
	}
}

void CScene::PrepareRender(ID3D12GraphicsCommandList* commandList)
{
	// Set
	commandList->SetGraphicsRootSignature(graphics_root_signature.Get());
}

void CScene::Render(ID3D12GraphicsCommandList* commandList , CCamera* camera)
{
	PrepareRender(commandList);

	camera->SetViewportsAndScissorRects(commandList);
	if (camera) camera->UpdateShaderVariables(commandList);

	for (int i = 0; i < objects.size(); ++i) {
		if (objects[i]) {
			objects[i]->Render(commandList);
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
