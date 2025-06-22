#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer(CCamera* otherCamera, float width, float height)
{
	camera.reset(otherCamera);
	camera->SetViewport(0, 0, width, height);
	camera->SetScissorRect(0, 0, width, height);
	camera->GenerateProjectionMatrix(1.0f, 500.0f, float(width) / float(height), 90.0f);
}

void CPlayer::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	CGameObject::CreateShaderVariables(device, commandList);
	if (camera) camera->CreateShaderVariables(device, commandList);
}

void CPlayer::Move(const XMFLOAT3& shift, bool updateVelocity)
{
	if (updateVelocity) {
		velocity = Vector3::Add(velocity, shift);
	}
	else {
		position = Vector3::Add(position, shift);
		if (camera) camera->Move(shift);
	}
}

void CPlayer::Move(DWORD direction, float distance)
{
	if (direction) {
		XMFLOAT3 shift = XMFLOAT3(0, 0, 0);
		if (direction & DIR_FORWARD) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&look), distance)));
		if (direction & DIR_BACKWARD) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&look), -distance)));
		if (direction & DIR_RIGHT) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&right), distance)));
		if (direction & DIR_LEFT) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&right), -distance)));
		if (direction & DIR_UP) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&up), distance)));
		if (direction & DIR_DOWN) XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift), XMVectorScale(XMLoadFloat3(&up), -distance)));

		Move(shift, true);
	}
}

void CPlayer::Rotate(float pitch, float yaw, float roll)
{
	if (camera) camera->Rotate(pitch, yaw, roll);

	if (pitch != 0.0f) {
		XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(pitch));
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), rotate));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotate));
	}
	if (yaw != 0.0f) {
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&up), XMConvertToRadians(yaw));
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), xmmtxRotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), xmmtxRotate));
	}
	if (roll != 0.0f) {
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&look), XMConvertToRadians(roll));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), xmmtxRotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), xmmtxRotate));
	}

	XMVECTOR xmvLook = XMVector3Normalize(XMLoadFloat3(&look));
	XMVECTOR xmvUp = XMVector3Normalize(XMLoadFloat3(&up));
	XMVECTOR xmvRight = XMVector3Normalize(XMVector3Cross(xmvUp, xmvLook));
	xmvUp = XMVector3Normalize(XMVector3Cross(xmvLook, xmvRight));

	XMStoreFloat3(&right, xmvRight);
	XMStoreFloat3(&up, xmvUp);
	XMStoreFloat3(&look, xmvLook);
}

void CPlayer::Update(float timeElapsed)
{
	Move(velocity, false);

	camera->Update(position, timeElapsed);
	camera->GenerateViewMatrix();

	// 감속
	XMVECTOR xmvVelocity = XMLoadFloat3(&velocity);
	XMVECTOR xmvDeceleration = XMVector3Normalize(XMVectorScale(xmvVelocity, -1.0f));
	float length = XMVectorGetX(XMVector3Length(xmvVelocity));
	float deceleration = friction * timeElapsed;
	if (deceleration > length) deceleration = length;
	XMStoreFloat3(&velocity, XMVectorAdd(xmvVelocity, XMVectorScale(xmvDeceleration, deceleration)));
}

void CPlayer::OnUpdateTransform()
{
	world_matrix._11 = right.x; world_matrix._12 = right.y; world_matrix._13 = right.z;
	world_matrix._21 = up.x; world_matrix._22 = up.y; world_matrix._23 = up.z;
	world_matrix._31 = look.x; world_matrix._32 = look.y; world_matrix._33 = look.z;
	world_matrix._41 = position.x; world_matrix._42 = position.y; world_matrix._43 = position.z;
}

void CPlayer::Animate(float elapsedTime)
{
	OnUpdateTransform();
}

void CPlayer::Render(ID3D12GraphicsCommandList* commandList)
{
	camera->SetViewportsAndScissorRects(commandList);
	camera->UpdateShaderVariables(commandList);

	CGameObject::Render(commandList);
}

CRollerCoasterPlayer::CRollerCoasterPlayer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, float width, float height) : CPlayer(new CFirstPersonCamera(this), width, height)
{
	std::shared_ptr<CMesh> cubeMesh = std::make_shared<CCubeMeshDiffused>(device, commandList);
	SetMesh(cubeMesh);
}

CTankPlayer::CTankPlayer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, float width, float height) : CPlayer(new CThirdPersonCamera(this), width, height)
{
	std::shared_ptr<CMesh> cubeMesh = std::make_shared<CCubeMeshDiffused>(device, commandList);
	SetMesh(cubeMesh);
}

void CTankPlayer::Animate(float elapsedTime)
{
	CPlayer::Animate(elapsedTime);

	for (CBulletObject& bullet : bullets) {
		bullet.Animate(elapsedTime);
	}
}

void CTankPlayer::Render(ID3D12GraphicsCommandList* commandList)
{
	CGameObject::Render(commandList);

	for (CBulletObject& bullet : bullets) {
		bullet.Render(commandList);
	}
}

CTerrainPlayer::CTerrainPlayer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, float width, float height, void* context)
	: CPlayer(new CThirdPersonCamera(this), width, height)
{
	std::shared_ptr<CMesh> cubeMesh = std::make_shared<CTankMesh>(device, commandList);
	SetMesh(cubeMesh);
	terrain = (CHeightMapTerrain*)context;
	float mapHeight = terrain->GetHeight(terrain->GetWidth() * 0.5f,terrain->GetLength() * 0.5f);
	position = (XMFLOAT3(terrain->GetWidth() * 0.5f, mapHeight + 50, terrain->GetLength() * 0.5f));
	CreateShaderVariables(device, commandList);
}

void CTerrainPlayer::Update(float elapsedTime)
{
	CPlayer::Update(elapsedTime);
	// 중력 적용
	gravity.Apply(position, velocity, elapsedTime);
	gravity.ResolveCollision(position, velocity, terrain->GetHeight(position.x, position.z));
}
