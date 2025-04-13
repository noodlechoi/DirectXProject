#pragma once
#include "stdafx.h"
#include "Object.h"

class CGameFramework {
public:

	void Create(HINSTANCE, HWND);
	void Delete();
	void BuildFrameBuffer();
	void ReleaseObjects();
	void ProcessFrame();
	void ClearFrameBuffer(DWORD);
	void AnimateObjects();
	void BuildObjects();
	void PresentFrameBuffer();
private:
	HINSTANCE h_instance{};
	HWND h_wnd{};


	HDC hdc_frame_buffer{};
	HBITMAP h_bitmap_frame_buffer{};

	RECT rect_client{};

	// 우선 하나만 구현
	std::unique_ptr<CObject> object;
};

