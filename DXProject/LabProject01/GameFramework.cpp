#include "GameFramework.h"


void CGameFramework::Create(HINSTANCE hInstance, HWND hMainWnd)
{
	h_instance = hInstance;
	h_wnd = hMainWnd;

	BuildFrameBuffer();

	BuildObjects();
}

void CGameFramework::Delete()
{
	ReleaseObjects();

	if (h_bitmap_frame_buffer) DeleteObject(h_bitmap_frame_buffer);
	if (hdc_frame_buffer) DeleteObject(hdc_frame_buffer);
}

void CGameFramework::BuildFrameBuffer()
{
	GetClientRect(h_wnd, &rect_client);

	HDC hDC = GetDC(h_wnd);

	hdc_frame_buffer = CreateCompatibleDC(hDC);
	h_bitmap_frame_buffer = CreateCompatibleBitmap(hDC, rect_client.right - rect_client.left, rect_client.bottom - rect_client.top);
	SelectObject(hdc_frame_buffer, h_bitmap_frame_buffer);

	ReleaseDC(h_wnd, hDC);
	SetBkMode(hdc_frame_buffer, TRANSPARENT);
}

void CGameFramework::ReleaseObjects()
{
}

void CGameFramework::ProcessFrame()
{
	//input_manager.InputProcess();

	//AnimateObjects();

	ClearFrameBuffer(RGB(255, 255, 255));

	object->Render(hdc_frame_buffer);

	PresentFrameBuffer();
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HPEN hPen = CreatePen(PS_SOLID, 0, dwColor);
	HPEN hOldPen = (HPEN)SelectObject(hdc_frame_buffer, hPen);
	HBRUSH hBrush = CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc_frame_buffer, hBrush);
	Rectangle(hdc_frame_buffer, rect_client.left, rect_client.top, rect_client.right, rect_client.bottom);
	SelectObject(hdc_frame_buffer, hOldPen);
	SelectObject(hdc_frame_buffer, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void CGameFramework::AnimateObjects()
{

}

void CGameFramework::BuildObjects()
{
	std::random_device rd;
	auto rm = std::mt19937(rd());

	auto uid_size = std::uniform_int_distribution<int>{ 20, 100 };
	auto uid_pos = std::uniform_int_distribution<int>{ 20, FRAME_BUFFER_WIDTH };
	auto uid_color = std::uniform_int_distribution<int>{0, 255};
	int x = uid_pos(rd);
	int y = uid_pos(rd);
	int nHalfWidth = uid_size(rd);
	int nHalfHeight = uid_size(rd);

	RECT r = { x - nHalfWidth, y - nHalfHeight, x + nHalfWidth, y + nHalfHeight };
	object = std::make_unique < CRectangleObject > (r);
}

void CGameFramework::PresentFrameBuffer()
{
	HDC hDC = GetDC(h_wnd);
	BitBlt(hDC, rect_client.left, rect_client.top, rect_client.right - rect_client.left, rect_client.bottom - rect_client.top, hdc_frame_buffer, rect_client.left, rect_client.top, SRCCOPY);
	ReleaseDC(h_wnd, hDC);
}
