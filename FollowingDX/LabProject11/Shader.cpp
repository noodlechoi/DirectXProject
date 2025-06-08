#include "stdafx.h"
#include "Object.h"
#include "Mesh.h"
#include "Shader.h"

CShader::CShader() 
{
}

CShader::~CShader()
{
}

CShader::CShader(CShader&& other) 
	: ref_num{ other.ref_num }, pipeline_states{ std::move(other.pipeline_states) }
{
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	return inputLayoutDesc;
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
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

	return rasterizerDesc;
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
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

	return blendDesc;
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0x00;
	depthStencilDesc.StencilWriteMask = 0x00;
	depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return depthStencilDesc;
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** shaderBlob)
{
	D3D12_SHADER_BYTECODE shaderBytecode{};
	return shaderBytecode;
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** shaderBlob)
{
	D3D12_SHADER_BYTECODE shaderBytecode{};
	return shaderBytecode;
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR* fileName, LPCSTR shaderName, LPCSTR shaderProfile, ID3DBlob** shaderBlob)
{
	UINT compileFlags{};
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	D3DCompileFromFile(fileName, nullptr, nullptr, shaderName, shaderProfile, compileFlags, 0, shaderBlob, nullptr);

	D3D12_SHADER_BYTECODE shaderBytecode{};
	shaderBytecode.BytecodeLength = (*shaderBlob)->GetBufferSize();
	shaderBytecode.pShaderBytecode = (*shaderBlob)->GetBufferPointer();

	return shaderBytecode;
}

void CShader::CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature)
{
	//그래픽스 파이프라인 상태 객체 배열을 생성한다.
	ComPtr<ID3DBlob> vertexShaderBlob{}, pixelShaderBlob{};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.pRootSignature = rootSignature;
	pipelineStateDesc.VS = CreateVertexShader(&vertexShaderBlob);
	pipelineStateDesc.PS = CreatePixelShader(&pixelShaderBlob);
	pipelineStateDesc.RasterizerState = CreateRasterizerState();
	pipelineStateDesc.BlendState = CreateBlendState();
	pipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	pipelineStateDesc.InputLayout = CreateInputLayout();
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	device->CreateGraphicsPipelineState(&pipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)pipeline_states.GetAddressOf());

	if (pipelineStateDesc.InputLayout.pInputElementDescs) delete[] pipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateShaderVariables(ID3D12Device*, ID3D12GraphicsCommandList*)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList*)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList, XMFLOAT4X4* worldMatrix)
{
	XMFLOAT4X4 TWorldMatrix;
	XMStoreFloat4x4(&TWorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(worldMatrix)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &TWorldMatrix, 0);
}

void CShader::ReleaseUploadBuffers()
{
}


void CShader::OnPrepareRender(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetPipelineState(pipeline_states.Get());
}

void CShader::Render(ID3D12GraphicsCommandList* commandList)
{
	OnPrepareRender(commandList);

}

CDiffusedShader::CDiffusedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CDiffusedShader::CreateInputLayout()
{
	const UINT inputElementDescNum = 2;
	D3D12_INPUT_ELEMENT_DESC* inputElementDescs = new D3D12_INPUT_ELEMENT_DESC[inputElementDescNum];

	//정점은 위치 벡터(POSITION)와 색상(COLOR)을 가진다. 
	inputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = inputElementDescNum;

	return inputLayoutDesc;
}

D3D12_SHADER_BYTECODE CDiffusedShader::CreateVertexShader(ID3DBlob** shaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", shaderBlob));
}

D3D12_SHADER_BYTECODE CDiffusedShader::CreatePixelShader(ID3DBlob** shaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", shaderBlob));
}

CObjectShader::CObjectShader()
{
}

CObjectShader::CObjectShader(float width, float height)
{
}

CObjectShader::CObjectShader(CObjectShader&& other) : CShader(std::move(other)), objects{ std::move(other.objects) }
{
}

void CObjectShader::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	player = std::make_shared<CSpaceShipPlayer>(device, commandList, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	CreateShaderVariables(device, commandList);
}

void CObjectShader::AnimateObjects(float fTimeElapsed)
{
	for (auto& object : objects) {
		if (object)	{
			object->Animate(fTimeElapsed);
		}
	}
}

void CObjectShader::Update(float elapsedTime)
{
	player->Update(elapsedTime);
}

D3D12_INPUT_LAYOUT_DESC CObjectShader::CreateInputLayout()
{
	const UINT inputElementDescNum = 2;
	D3D12_INPUT_ELEMENT_DESC* inputElementDescs = new D3D12_INPUT_ELEMENT_DESC[inputElementDescNum];

	//정점은 위치 벡터(POSITION)와 색상(COLOR)을 가진다. 
	inputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = inputElementDescNum;

	return inputLayoutDesc;
}

D3D12_SHADER_BYTECODE CObjectShader::CreateVertexShader(ID3DBlob** shaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", shaderBlob));
}

D3D12_SHADER_BYTECODE CObjectShader::CreatePixelShader(ID3DBlob** shaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", shaderBlob));
}

void CObjectShader::ReleaseUploadBuffers()
{
	for (auto& object : objects) {
		if (object) {
			object->ReleaseUploadBuffers();
		}
	}
}

void CObjectShader::PlayerMove(DWORD direction, float distance)
{
	player->Move(direction, distance);
}

void CObjectShader::PlayerRotate(float pitch, float yaw, float roll)
{
	player->Rotate(pitch, yaw, roll);
}

void CObjectShader::Render(ID3D12GraphicsCommandList* commandList)
{
	CShader::Render(commandList);

	player->Render(commandList);

	for (auto& object : objects) {
		if (object) {
			object->Render(commandList);
		}
	}
}
