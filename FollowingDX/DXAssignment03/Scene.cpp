#include "stdafx.h"
#include "Scene.h"

CScene::CScene(CInputManager* inputManager, float width, float height)
	: cliend_width{ width }, cliend_height{ height }
{
	input_manager.reset(inputManager);

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
	if(shader){
		shader->ReleaseUploadBuffers();
	}
}

void CScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	graphics_root_signature = CreateGraphicsRootSignature(device);

	shader = std::make_unique<CObjectShader>(cliend_width, cliend_height);
	shader->CreateShader(device, graphics_root_signature.Get());
	shader->BuildObjects(device, commandList);
}

void CScene::ProcessInput(HWND& hWnd, float elapsedTime)
{
	input_manager->ProcessInput(hWnd, shader.get());
	shader->Update(elapsedTime);
}

void CScene::AnimateObjects(float elapsedTime)
{
	if (shader) {
		shader->AnimateObjects(elapsedTime);
	}
}

void CScene::PrepareRender(ID3D12GraphicsCommandList* commandList)
{
	// Set
	commandList->SetGraphicsRootSignature(graphics_root_signature.Get());
}

void CScene::Render(ID3D12GraphicsCommandList* commandList )
{
	PrepareRender(commandList);

	if(shader) {
		shader->Render(commandList);
	}
}

bool CScene::OnProcessMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	input_manager->ProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	return true;
}

bool CScene::OnProcessKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	input_manager->ProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	return true;
}

CRollerCoasterScene::CRollerCoasterScene(float width, float height)
	: CScene(new CRollerCoasterInputManager, width, height)
{
}

void CRollerCoasterScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	graphics_root_signature = CreateGraphicsRootSignature(device);

	shader = std::make_unique<CObjectShader>(cliend_width, cliend_height);
	shader->CreateShader(device, graphics_root_signature.Get());
	shader->RollerCoasterBuildObjects(device, commandList);
}

CScene* CRollerCoasterScene::NextScene()
{
	return new CTankScene(cliend_width, cliend_height);
}

CTankScene::CTankScene(float width, float height)
	: CScene(new CTankInputManager, width, height)
{
}

void CTankScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	graphics_root_signature = CreateGraphicsRootSignature(device);

	shader = std::make_unique<CObjectShader>(cliend_width, cliend_height);
	shader->CreateShader(device, graphics_root_signature.Get());
	shader->TankBuildObjects(device, commandList);
	//shader[0].BuildObjects(device, commandList);
}

void CTankScene::CheckObjectByBulletCollisions()
{

}

void CTankScene::AnimateObjects(float elapsedTime)
{
	CScene::AnimateObjects(elapsedTime);
	CheckObjectByBulletCollisions();
}

CTerrainScene::CTerrainScene(float width, float height)
	: CScene(new CTerrainInputManager, width, height)
{
}

void CTerrainScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	graphics_root_signature = CreateGraphicsRootSignature(device);

	shader = std::make_unique<CTerrainShader>();
	shader->CreateShader(device, graphics_root_signature.Get());
	shader->BuildObjects(device, commandList);
}