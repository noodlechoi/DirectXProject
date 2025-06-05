#include "stdafx.h"
#include "Scene.h"
CScene::CScene()
{
}

CScene::~CScene()
{
	ReleaseObjects();
}


void CScene::CreateGraphicsRootSignature(ID3D12Device* device)
{
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
	device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)graphics_root_signature.GetAddressOf());
}

void CScene::CreateGraphicsPiplineState(ID3D12Device* device)
{
	ComPtr<ID3DBlob> vertexShaderBlob{};
	ComPtr<ID3DBlob> pixelShaderBlob{};
	ComPtr<ID3DBlob> errorBlob{};

	UINT compileFlags{};
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_1", compileFlags, 0, vertexShaderBlob.GetAddressOf(), errorBlob.GetAddressOf());
	D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_1", compileFlags, 0, pixelShaderBlob.GetAddressOf(), nullptr);

	// 래스터라이저 상태 설정
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// 블렌드 상태 설정
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// 그래픽 파이프라인 상태 설정
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.pRootSignature = graphics_root_signature.Get();
	pipelineStateDesc.VS.pShaderBytecode = vertexShaderBlob->GetBufferPointer();
	pipelineStateDesc.VS.BytecodeLength = vertexShaderBlob->GetBufferSize();
	pipelineStateDesc.PS.pShaderBytecode = pixelShaderBlob->GetBufferPointer();
	pipelineStateDesc.PS.BytecodeLength = pixelShaderBlob->GetBufferSize();
	pipelineStateDesc.RasterizerState = rasterizerDesc;
	pipelineStateDesc.BlendState = blendDesc;
	pipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	pipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	pipelineStateDesc.InputLayout.NumElements = 0;
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.SampleDesc.Quality = 0;
	device->CreateGraphicsPipelineState(&pipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)pipeline_state.GetAddressOf());
}

void CScene::BuildObjects(ID3D12Device* device)
{
	CreateGraphicsRootSignature(device);
	CreateGraphicsPiplineState(device);
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

}

void CScene::PrepareRender(ID3D12GraphicsCommandList* commandList)
{
	// Set
	commandList->SetGraphicsRootSignature(graphics_root_signature.Get());
	commandList->SetPipelineState(pipeline_state.Get());
	// 삼각형 릿트 설정
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CScene::Render(ID3D12GraphicsCommandList* commandList)
{
	PrepareRender(commandList);

	commandList->DrawInstanced(6, 1, 0, 0);
}

bool CScene::OnProcessMouseMessage(HWND, UINT, WPARAM, LPARAM)
{
	return false;
}

bool CScene::OnProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM)
{
	return false;
}
