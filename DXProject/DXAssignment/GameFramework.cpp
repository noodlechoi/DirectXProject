#include "stdafx.h"
#include "GameFramework.h"

bool IsNextScene{};

CGameFramework::CGameFramework() : now_scene{std::make_unique<CRollerCoasterScene>()}
{
}

void CGameFramework::Create(HINSTANCE hInstance, HWND hMainWnd)
{
	h_instance = hInstance;
	h_wnd = hMainWnd;

	BuildFrameBuffer();

	BuildObjects();

	_tcscpy_s(frame_rate_title, _T("LabProject ("));
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
	timer.Tick(60);

	ClearFrameBuffer(RGB(255, 255, 255));

	AnimateObjects();

	if (now_scene) {
		now_scene->ProcessInput(h_wnd, timer.GetTimeElapsed());
		now_scene->Render(hdc_frame_buffer);
		
	}

	PresentFrameBuffer();

	timer.GetFrameRate(frame_rate_title + 12, 37);
	::SetWindowText(h_wnd, frame_rate_title);
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
	float timeElapsed = timer.GetTimeElapsed();
	if (now_scene) now_scene->Animate(timeElapsed);
	if (IsNextScene) {
		now_scene.reset(now_scene->NextScene());
		now_scene->BuildObjects();
		IsNextScene = false;
	}
}

void CGameFramework::BuildObjects()
{
	now_scene = std::make_unique<CRollerCoasterScene>();
	now_scene->BuildObjects();
}

void CGameFramework::PresentFrameBuffer()
{
	HDC hDC = GetDC(h_wnd);
	BitBlt(hDC, rect_client.left, rect_client.top, rect_client.right - rect_client.left, rect_client.bottom - rect_client.top, hdc_frame_buffer, rect_client.left, rect_client.top, SRCCOPY);
	ReleaseDC(h_wnd, hDC);
}

LRESULT CGameFramework::ProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return now_scene->ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}
