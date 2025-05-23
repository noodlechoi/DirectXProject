#include "stdafx.h"
#include "GameFramework.h"


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
	timer.Tick(10);

	//input_manager.InputProcess();

	AnimateObjects();

	ClearFrameBuffer(RGB(255, 255, 255));

	for(const auto& object : objects)
		object->Render(hdc_frame_buffer);

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
	// 랜덤 위치, 색상 설정
	std::random_device rd;
	auto rm = std::mt19937(rd());

	auto uid_pos = std::uniform_int_distribution<int>{ 20, FRAME_BUFFER_WIDTH };
	auto uid_color = std::uniform_int_distribution<int>{ 0, 255 };
	
	// 오브젝트 랜덤 값 재할당
	for (auto& object : objects) {
		Point p = { uid_pos(rd), uid_pos(rd) };
		object->SetPosition(p);
		object->SetColor(RGB(uid_color(rd), uid_color(rd), uid_color(rd)));
	}
}

void CGameFramework::BuildObjects()
{
	// 랜덤 위치, 사이즈 설정
	std::random_device rd;
	auto rm = std::mt19937(rd());

	auto uid_size = std::uniform_int_distribution<int>{ 20, 100 };
	auto uid_pos = std::uniform_int_distribution<int>{ 20, FRAME_BUFFER_WIDTH };
	
	

	for (int i = 0; i < object_num; ++i) {
		// 위치 사이즈 정의
		int x = uid_pos(rd);
		int y = uid_pos(rd);
		int nHalfWidth = uid_size(rd);
		int nHalfHeight = uid_size(rd);

		// 오브젝트 생성
		int type = (i % 5);
		RECT r = { x - nHalfWidth, y - nHalfHeight, x + nHalfWidth, y + nHalfHeight };
		switch (type) {
		case (int)CObject::eType::Rectangle:
			objects[i] = std::make_unique<CRectangleObject>(r);
			break;
		case (int)CObject::eType::RoundRectangle:
			objects[i] = std::make_unique<CRoundRectangleObject>(r);
			break;
		case (int)CObject::eType::Ellipse:
			objects[i] = std::make_unique<CEllipseObject>(r);
			break;
		case (int)CObject::eType::Star:
			objects[i] = std::make_unique<CStarObject>(r);
			break;
		case (int)CObject::eType::Hexagon:
			objects[i] = std::make_unique<CHexagonObject>(r);
			break;
		default:
			break;
		}
	}
}

void CGameFramework::PresentFrameBuffer()
{
	HDC hDC = GetDC(h_wnd);
	BitBlt(hDC, rect_client.left, rect_client.top, rect_client.right - rect_client.left, rect_client.bottom - rect_client.top, hdc_frame_buffer, rect_client.left, rect_client.top, SRCCOPY);
	ReleaseDC(h_wnd, hDC);
}

LRESULT CGameFramework::ProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return input_manager.ProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
}
