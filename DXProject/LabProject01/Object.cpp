#include "Object.h"

CRectangleObject::CRectangleObject(const RECT& rect)
{
	n_type = eType::Rectangle;

	size.width = rect.right - rect.left;
	size.height = rect.bottom - rect.top;
	SetPosition({int(size.width * 0.5f), int(size.height * 0.5f)});
}

void CRectangleObject::Render(HDC hdcFramebuffer)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HBRUSH hBrush = ::CreateSolidBrush(dw_color);
	HPEN hOldPen = (HPEN)::SelectObject(hdcFramebuffer, hPen);
	HPEN hOldBrush = (HPEN)::SelectObject(hdcFramebuffer, hBrush);
	Size half_size = { int(size.width * 0.5f), int(size.height * 0.5f) };
	::Rectangle(hdcFramebuffer, pos.x - half_size.width, pos.y - half_size.height, pos.x + half_size.width, pos.y + half_size.height);
	::SelectObject(hdcFramebuffer, hOldPen);
	::SelectObject(hdcFramebuffer, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}