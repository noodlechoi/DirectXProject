#pragma once
#include "GameObject.h"

// pipeline 상태 객체를 생성하고 게임 오브젝트를 관리하는 클래스
class CShader
{
public:
	CShader();
	virtual ~CShader();

	void AddRef() { ref_num++; }
	void Release() { if (--ref_num <= 0) delete this; }

	// 입력 조립기에게 정점 버퍼를 알려주기 위한 구조체 반환
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	// 래스터라이저 상태를 설정하기 위한 구조체 반환
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	// 블렌드 상태를 설정하기 위한 구조체 반환
	virtual D3D12_BLEND_DESC CreateBlendState();
	// 깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체 반환
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	// 셰이더 코드 컴파일
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** );
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** );
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* , LPCSTR ,LPCSTR , ID3DBlob** );

	//그래픽스 파이프라인 상태 객체 생성
	virtual void CreateShader(ID3D12Device* , ID3D12RootSignature* );
	virtual void CreateShaderVariables(ID3D12Device*, ID3D12GraphicsCommandList*);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList*);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void BuildObjects(ID3D12Device* , ID3D12GraphicsCommandList*, void* = nullptr);
	virtual void AnimateObjects(float );

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* );
	virtual void Render(ID3D12GraphicsCommandList*);
private:
	size_t ref_num{};

protected:
	std::deque<std::unique_ptr<CGameObject>> objects{};
	std::deque<ComPtr<ID3D12PipelineState>> pipeline_states{};
};

