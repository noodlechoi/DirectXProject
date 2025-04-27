#pragma once
#include "Object.h"
#include "InputManager.h"
#include "Timer.h"

class CGameFramework {
public:
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
	HINSTANCE h_instance{};
	HWND h_wnd{};
	CInputManager input_manager{};

	HDC hdc_frame_buffer{};
	HBITMAP h_bitmap_frame_buffer{};

	RECT rect_client{};

	CTimer timer;
	_TCHAR frame_rate_title[50];

	const static size_t object_num{100};
	std::array<std::unique_ptr<CObject>, object_num> objects;
};

