#pragma once
#include "Player.h"

// ���� ��ü�� ������ ���̴����� �Ѱ��ֱ� ���� ��� ���� ����ü
//struct CB_GAMEOBJECT_INFO {
//	XMFLOAT4X4 world_matrix; // ���� ���
//};


// pipeline ���� ��ü�� ����
class CShader
{
public:
	CShader();
	virtual ~CShader();
	CShader(CShader&&);

	void AddRef() { ref_num++; }
	void Release() { if (--ref_num <= 0) delete this; }

	// �Է� �����⿡�� ���� ���۸� �˷��ֱ� ���� ����ü ��ȯ
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() = 0;
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
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList*, XMFLOAT4X4*);
	virtual void ReleaseUploadBuffers();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* );
	virtual void Render(ID3D12GraphicsCommandList*);
private:
	size_t ref_num{};

protected:
	ComPtr<ID3D12PipelineState> pipeline_states{};
};

class CDiffusedShader : public CShader
{
public:
	CDiffusedShader();

	D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob**) override;
	D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob**) override;
};

class CObjectShader : public CShader
{
public:
	CObjectShader();
	CObjectShader(float, float);
	virtual ~CObjectShader() {}
	CObjectShader(CObjectShader&&);

	virtual void BuildObjects(ID3D12Device* , ID3D12GraphicsCommandList* );
	void RollerCoasterBuildObjects(ID3D12Device* , ID3D12GraphicsCommandList* );
	void TankBuildObjects(ID3D12Device* , ID3D12GraphicsCommandList* );
	
	virtual void AnimateObjects(float );
	virtual void Update(float);

	D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ) override;
	D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ) override;
	void ReleaseUploadBuffers() override;

	void PlayerMove(DWORD, float);
	void PlayerRotate(float, float, float);
	void Render(ID3D12GraphicsCommandList*) override;
protected:
	std::deque<std::unique_ptr<CGameObject>> objects{};
	std::shared_ptr<CPlayer> player{};
};

class CTerrainShader : public CObjectShader {
public:
	CTerrainShader();
	virtual ~CTerrainShader() {};
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	void BuildObjects(ID3D12Device*, ID3D12GraphicsCommandList*) override;
	void Update(float) override;
	void Render(ID3D12GraphicsCommandList*) override;

	CHeightMapTerrain* GetTerrain() { return terrain.get(); }
protected:
	ID3D12GraphicsCommandList* command_list{};
	ID3D12Device* d3d_device{};

	std::shared_ptr<CHeightMapTerrain> terrain{};
};