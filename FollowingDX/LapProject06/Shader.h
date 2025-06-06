#pragma once
#include "GameObject.h"

// pipeline ���� ��ü�� �����ϰ� ���� ������Ʈ�� �����ϴ� Ŭ����
class CShader
{
public:
	CShader();
	virtual ~CShader();

	void AddRef() { ref_num++; }
	void Release() { if (--ref_num <= 0) delete this; }

	// �Է� �����⿡�� ���� ���۸� �˷��ֱ� ���� ����ü ��ȯ
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	// �����Ͷ����� ���¸� �����ϱ� ���� ����ü ��ȯ
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	// ���� ���¸� �����ϱ� ���� ����ü ��ȯ
	virtual D3D12_BLEND_DESC CreateBlendState();
	// ����-���ٽ� �˻縦 ���� ���¸� �����ϱ� ���� ����ü ��ȯ
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	// ���̴� �ڵ� ������
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** );
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** );
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* , LPCSTR ,LPCSTR , ID3DBlob** );

	//�׷��Ƚ� ���������� ���� ��ü ����
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

