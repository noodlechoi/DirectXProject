#pragma once

class CViewport {
public:
	int left{}, top{}, width{}, height{};
	
	void SetViewport(int, int , int , int );
};

class CPlayer;

class CCamera
{
public:
	CCamera();

	void SetFOVAngle(float);

	void GenerateViewMatrix();
	void GeneratePerspectiveProjectionMatrix(float , float , float );
	void SetViewport(int , int , int , int );

	void SetLookAt(XMFLOAT3& , XMFLOAT3& );
	void SetLookAt(XMFLOAT3& , XMFLOAT3& , XMFLOAT3& );

	void Move(XMFLOAT3& );
	void Move(XMFLOAT3&& );
	void Move(float , float , float );
	void Rotate(float  = 0.0f, float  = 0.0f, float  = 0.0f);
	//void Update(std::unique_ptr<CPlayer> , XMFLOAT3& , float  = 0.016f);
private:
	XMFLOAT3 position{ XMFLOAT3(0.0f, 0.0f, 0.0f) };
	XMFLOAT3 right{ XMFLOAT3(1.0f, 0.0f, 0.0f) };
	XMFLOAT3	up{ XMFLOAT3(0.0f, 1.0f, 0.0f) };
	XMFLOAT3 look{ XMFLOAT3(0.0f, 0.0f, 1.0f) };

	float fov_angle{ 90.0f };
	float project_distance{ 1.0f };

	float aspect_ratio{ float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT) };
public:
	CViewport viewport;

	XMFLOAT4X4 view_matrix;
	XMFLOAT4X4 perspective_project_matrix;
	XMFLOAT4X4 view_perspective_project_matrix;
};

