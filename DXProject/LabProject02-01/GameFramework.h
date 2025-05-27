#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"

class CGameFramework {
public:
	CGameFramework();

	void Create(HINSTANCE, HWND);
	void Delete();

	void BuildFrameBuffer();
	void ReleaseObjects();

	void ProcessFrame();
	void ClearFrameBuffer(DWORD);
	void PresentFrameBuffer();
	LRESULT CALLBACK ProcessingWindowMessage(HWND, UINT , WPARAM , LPARAM );

	void AnimateObjects();
	void BuildObjects();

private:
	// window
	HINSTANCE h_instance{};
	HWND h_wnd{};

	HDC hdc_frame_buffer{};
	HBITMAP h_bitmap_frame_buffer{};

	RECT rect_client{};

	// timer
	CTimer timer;
	_TCHAR frame_rate_title[50];

	//scene
	std::unique_ptr<CScene> now_scene;
};

