#include "stdafx.h"
#include "Scene.h"

extern HWND ghWnd;


void CScene::ReleaseUploadBuffers()
{
	for (const auto& object : objects) {
		object->ReleaseUploadBuffer();
	}
}

void CScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	// 카메라 객체 생성
	RECT client_rect;
	GetClientRect(ghWnd, &client_rect);
	float width{ float(client_rect.right - client_rect.left) };
	float height{ float(client_rect.bottom- client_rect.top)};

	camera = std::make_unique<CCamera>();
	camera->SetViewport(0, 0, width, height);
	camera->SetScissorRect(0, 0, width, height);
	camera->GenerateProjectionMatrix(1.0f, 500.0f, (float)width / (float)height, 90.0f);
	camera->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	camera->SetCameraOffset(XMFLOAT3(0.0f, 2.0f, -2.0f));

	auto object = std::make_unique<CBillboardObject>();
	object->SetPosition(XMFLOAT3(2.0f, 0.0f, 0.0f));

	//CTriangleMesh* mesh= new CTriangleMesh(device, commandList);
	//CRectangleMesh* mesh= new CRectangleMesh(device, commandList);
	CCubeMesh* mesh= new CCubeMesh(device, commandList);
	object->SetMesh(mesh);
	CTexture* tex = new CTexture(std::string("stone"));
	tex->CreateTextureResource(device, commandList, std::wstring(L"Image\\Stone01.dds"));
	object->SetTexture(tex);
	objects.push_back(std::move(object));

	shaders.push_back(std::make_unique<CTextureShader>());
	shaders[0]->CreateShader(device);
	shaders[0]->CreateShaderVariables(device, objects[0].get());

	tex->CreateSrv(device, shaders[0]->GetCPUDescriptorHandle());
}

void CScene::AnimateObjects(float elapsedTime)
{
	for (const auto& object : objects) {
		object->Animate(elapsedTime, camera.get());
	}
}


void CScene::Render(ID3D12GraphicsCommandList* commandList)
{
	for (const auto& shader : shaders) {
		shader->PreRender(commandList);	// root signature set
	}
	// camera set
	camera->SetViewportsAndScissorRects(commandList);
	camera->UpdateShaderVariables(commandList);
	// object set
	for (const auto& object : objects) {
		object->UpdateShaderVariables(commandList);
	}

	// setpipeline
	for (const auto& shader : shaders) {
		shader->Render(commandList);
	}
	for (const auto& object : objects) {
		object->Render(commandList);
	}
}

void CTitleScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	// camera
	RECT client_rect;
	GetClientRect(ghWnd, &client_rect);
	float width{ float(client_rect.right - client_rect.left) };
	float height{ float(client_rect.bottom - client_rect.top) };

	camera = std::make_unique<CCamera>();
	camera->SetViewport(0, 0, width, height);
	camera->SetScissorRect(0, 0, width, height);
	camera->GenerateProjectionMatrix(1.0f, 500.0f, (float)width / (float)height, 90.0f);
	camera->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	camera->SetCameraOffset(XMFLOAT3(0.0f, 0.0f, -1.0f));

	// object
	auto object = std::make_unique<CBillboardObject>();

	//CTriangleMesh* mesh= new CTriangleMesh(device, commandList);
	CRectangleMesh* mesh = new CRectangleMesh(device, commandList, 3.0f, 2.0f);
	//CCubeMesh* mesh = new CCubeMesh(device, commandList);
	object->SetMesh(mesh);
	CTexture* tex = new CTexture(std::string("title"));
	tex->CreateTextureResource(device, commandList, std::wstring(L"Image\\title.dds"));
	object->SetTexture(tex);
	objects.push_back(std::move(object));
	
	// shader
	shaders.push_back(std::make_unique<CTextureShader>());
	shaders[0]->CreateShader(device);
	shaders[0]->CreateShaderVariables(device, objects[0].get());

	tex->CreateSrv(device, shaders[0]->GetCPUDescriptorHandle());
}

extern bool nextScene;

void CTitleScene::ProcessInput()
{
	//input_manager->ProcessInput();

	static UCHAR key_buffer[256];
	if (GetKeyboardState(key_buffer))
	{
	}

	if (GetCapture() == ghWnd) {
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		ScreenToClient(ghWnd, &cursorPos);
		if (key_buffer[VK_LBUTTON] & 0xF0) {
			if (cursorPos.x >= start_button.left && cursorPos.x <= start_button.right && cursorPos.y >= start_button.top && cursorPos.y <= start_button.bottom) {
				nextScene = true;
			}
			else if(cursorPos.x >= exit_button.left && cursorPos.x <= exit_button.right && cursorPos.y >= exit_button.top && cursorPos.y <= exit_button.bottom) {
				::PostQuitMessage(0);
			}
		}
	}
}

// Game
void CGameScene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	// 카메라 객체 생성
	RECT client_rect;
	GetClientRect(ghWnd, &client_rect);
	float width{ float(client_rect.right - client_rect.left) };
	float height{ float(client_rect.bottom - client_rect.top) };

	camera = std::make_unique<CCamera>();
	camera->SetViewport(0, 0, width, height);
	camera->SetScissorRect(0, 0, width, height);
	camera->GenerateProjectionMatrix(1.0f, 500.0f, (float)width / (float)height, 90.0f);
	camera->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	camera->SetCameraOffset(XMFLOAT3(0.0f, 2.0f, -2.0f));

	auto object = std::make_unique<CBillboardObject>();
	object->SetPosition(XMFLOAT3(2.0f, 0.0f, 0.0f));

	//CTriangleMesh* mesh= new CTriangleMesh(device, commandList);
	CRectangleMesh* mesh= new CRectangleMesh(device, commandList);
	//CCubeMesh* mesh = new CCubeMesh(device, commandList);
	object->SetMesh(mesh);
	CTexture* tex = new CTexture(std::string("tree"));
	tex->CreateTextureResource(device, commandList, std::wstring(L"Image\\tree.dds"));
	object->SetTexture(tex);
	objects.push_back(std::move(object));

	shaders.push_back(std::make_unique<CTextureShader>());
	shaders[0]->CreateShader(device);
	shaders[0]->CreateShaderVariables(device, objects[0].get());

	tex->CreateSrv(device, shaders[0]->GetCPUDescriptorHandle());
}

extern POINT oldCursorPos;

void CGameScene::ProcessInput()
{
	static UCHAR key_buffer[256];
	if (GetKeyboardState(key_buffer))
	{
		XMFLOAT2 direction{};
		if (key_buffer[VK_UP] & 0xF0) direction.y++;
		if (key_buffer[VK_DOWN] & 0xF0) direction.y--;
		if (key_buffer[VK_LEFT] & 0xF0) direction.x--;
		if (key_buffer[VK_RIGHT] & 0xF0) direction.x++;

		if (direction.x != 0 || direction.y != 0) {
			camera->Move(direction, 0.005);
		}
	}

	if (GetCapture() == ghWnd) {
		SetCursor(NULL);
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		float cxMouseDelta = (float)(ptCursorPos.x - oldCursorPos.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - oldCursorPos.y) / 3.0f;
		SetCursorPos(oldCursorPos.x, oldCursorPos.y);
		if (cxMouseDelta || cyMouseDelta)
		{
			if (key_buffer[VK_LBUTTON] & 0xF0) {
				camera->Rotate(cyMouseDelta, cxMouseDelta, 0.0f);
			}
			if (key_buffer[VK_RBUTTON] & 0xF0)
				camera->Rotate(cyMouseDelta, 0.0f, -cxMouseDelta);
		}

	}
}
