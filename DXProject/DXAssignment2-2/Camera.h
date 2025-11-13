#pragma once

struct CB_Camera_INFO
{
	XMFLOAT4X4 view_matrix;
	XMFLOAT4X4 projection_matrix;
};

class CCamera
{
public:
	CCamera();

	virtual void CreateShaderVariables(ID3D12Device*, ID3D12GraphicsCommandList*);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList*);

	void GenerateViewMatrix(XMFLOAT3, XMFLOAT3, XMFLOAT3);
	void GenerateProjectionMatrix(float, float, float, float);
	void SetViewport(int, int, int, int, float = 0.0f, float = 1.0f);
	void SetScissorRect(LONG, LONG, LONG, LONG);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList*);
protected:
	XMFLOAT4X4 view_matrix;
	XMFLOAT4X4 projection_matrix;
	ComPtr<ID3D12Resource> viewproj_cb;
	std::unique_ptr<CB_Camera_INFO> mapped_camera_info;


	D3D12_VIEWPORT viewport;
	D3D12_RECT scissor_rect;
};


