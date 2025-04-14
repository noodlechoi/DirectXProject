#pragma once
#include "stdafx.h"
#include "Object.h"
#include "InputManager.h"

class CGameFramework {
public:
	void Create(HINSTANCE, HWND);
	void Delete();

	void BuildFrameBuffer();
	void ReleaseObjects();

	void ProcessFrame();
	void ClearFrameBuffer(DWORD);
	void PresentFrameBuffer();
	LRESULT CALLBACK ProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void AnimateObjects();
	void BuildObjects();

private:
	HINSTANCE h_instance{};
	HWND h_wnd{};
	CInputManager input_manager{};

	HDC hdc_frame_buffer{};
	HBITMAP h_bitmap_frame_buffer{};

	RECT rect_client{};

	const static size_t object_num{100};
	std::array<std::unique_ptr<CObject>, object_num> objects;
};

