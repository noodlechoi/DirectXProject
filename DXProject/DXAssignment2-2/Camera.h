#pragma once


class CCamera
{
public:
	CCamera();

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList*);

	//void GenerateViewMatrix(XMFLOAT3, XMFLOAT3, XMFLOAT3);
	void GenerateProjectionMatrix(float, float, float, float);
	void SetViewport(int, int, int, int, float = 0.0f, float = 1.0f);
	void SetScissorRect(LONG, LONG, LONG, LONG);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList*);

	void SetLookAt(XMFLOAT3, XMFLOAT3, XMFLOAT3);
	void SetCameraOffset(XMFLOAT3&);
	void GenerateViewMatrix();

	XMFLOAT3 GetPos() const { return position; }
	void Rotate(float pitch, float yaw, float roll);
	void Move(const XMFLOAT3 shift);
	void Move(const XMFLOAT2 direction, float distance);
protected:
	XMFLOAT4X4 view_matrix;
	XMFLOAT4X4 projection_matrix;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissor_rect;

	XMFLOAT3 position{ XMFLOAT3(0.0f, 0.0f, 0.0f) };
	XMFLOAT3 right{ XMFLOAT3(1.0f, 0.0f, 0.0f) };
	XMFLOAT3 up{ XMFLOAT3(0.0f, 1.0f, 0.0f) };
	XMFLOAT3 look{ XMFLOAT3(0.0f, 0.0f, 1.0f) };
	XMFLOAT3 offset{};
	XMFLOAT3 look_at{};

	BoundingFrustum frustum;
};


