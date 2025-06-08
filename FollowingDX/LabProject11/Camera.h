#pragma once

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class CPlayer;

//// 카메라 상수 버퍼를 위한 구조체
//struct VS_CB_CAMERA_INFO {
//	XMFLOAT4X4 view_matrix;
//	XMFLOAT4X4 projection_matrix;
//};

class CCamera
{
public:
	CCamera(CPlayer*);

	virtual void CreateShaderVariables(ID3D12Device*, ID3D12GraphicsCommandList*);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList*);

	void GenerateViewMatrix(XMFLOAT3 , XMFLOAT3 , XMFLOAT3 );
	void GenerateViewMatrix();
	void RegenerateViewMatrix();
	void GenerateProjectionMatrix(float , float , float	, float );
	void SetViewport(int , int , int , int , float = 0.0f, float = 1.0f);
	void SetScissorRect(LONG , LONG , LONG , LONG );
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* );

	void SetLookAt(XMFLOAT3&, XMFLOAT3&);
	void SetLookAt(XMFLOAT3, XMFLOAT3, XMFLOAT3);
	void SetCameraOffset(XMFLOAT3&);

	virtual void Move(const XMFLOAT3&);
	virtual void Rotate(float, float, float);
	virtual void Update(XMFLOAT3&, float);
protected:
	XMFLOAT4X4 view_matrix;
	XMFLOAT4X4 projection_matrix;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissor_rect;

	XMFLOAT3 position{ XMFLOAT3(0.0f, 0.0f, 0.0f) };
	XMFLOAT3 right{ XMFLOAT3(1.0f, 0.0f, 0.0f) };
	XMFLOAT3 up{ XMFLOAT3(0.0f, 1.0f, 0.0f) };
	XMFLOAT3 look{ XMFLOAT3(0.0f, 0.0f, 1.0f) };
	
	XMFLOAT3 look_at_world{};
	XMFLOAT3 offset{};
	float time_lag{};

	CPlayer* player;
};

class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera(CPlayer*);

	void Update(XMFLOAT3&, float) override;
};

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(CPlayer*);

	void Rotate(float, float, float) override;
	void Update(XMFLOAT3&, float) override;
};
