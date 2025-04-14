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

CRoundRectangleObject::CRoundRectangleObject(const RECT& rect) : CRectangleObject(rect)
{
	n_type = eType::RoundRectangle;
}

void CRoundRectangleObject::Render(HDC hdcFramebuffer)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HBRUSH hBrush = ::CreateSolidBrush(dw_color);
	HPEN hOldPen = (HPEN)::SelectObject(hdcFramebuffer, hPen);
	HPEN hOldBrush = (HPEN)::SelectObject(hdcFramebuffer, hBrush);
	Size half_size = { int(size.width * 0.5f), int(size.height * 0.5f) };
	::RoundRect(hdcFramebuffer, pos.x - half_size.width, pos.y - half_size.height, pos.x + half_size.width, pos.y + half_size.height, size.width, size.height);
	::SelectObject(hdcFramebuffer, hOldPen);
	::SelectObject(hdcFramebuffer, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

CEllipseObject::CEllipseObject(const RECT& rect)
{
	n_type = eType::Ellipse;

	size.width = rect.right - rect.left;
	size.height = rect.bottom - rect.top;
	SetPosition({ int(size.width * 0.5f), int(size.height * 0.5f) });
}

void CEllipseObject::Render(HDC hdcFramebuffer)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HBRUSH hBrush = ::CreateSolidBrush(dw_color);
	HPEN hOldPen = (HPEN)::SelectObject(hdcFramebuffer, hPen);
	HPEN hOldBrush = (HPEN)::SelectObject(hdcFramebuffer, hBrush);
	Size half_size = { int(size.width * 0.5f), int(size.height * 0.5f) };
	::Ellipse(hdcFramebuffer, pos.x - half_size.width, pos.y - half_size.height, pos.x + half_size.width, pos.y + half_size.height);
	::SelectObject(hdcFramebuffer, hOldPen);
	::SelectObject(hdcFramebuffer, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

CPolygonObject::CPolygonObject(const RECT& rect)
{
	SetPosition({ int((rect.right - rect.left) * 0.5f), int((rect.bottom - rect.top) * 0.5f) });
}

void CPolygonObject::SetVertices(const std::vector<POINT>& pVertices)
{
	p_vertices.assign(pVertices.begin(), pVertices.end());
}

void CPolygonObject::Render(HDC hdcFramebuffer)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HBRUSH hBrush = ::CreateSolidBrush(dw_color);
	HPEN hOldPen = (HPEN)::SelectObject(hdcFramebuffer, hPen);
	HPEN hOldBrush = (HPEN)::SelectObject(hdcFramebuffer, hBrush);
	std::vector<POINT>pVertices{ p_vertices };
	for (int i = 0; i < pVertices.size(); ++i) {
		pVertices[i].x = p_vertices[i].x + pos.x;
		pVertices[i].y = p_vertices[i].y + pos.y;
	}
	Polygon(hdcFramebuffer, pVertices.data(), pVertices.size());
	::SelectObject(hdcFramebuffer, hOldPen);
	::SelectObject(hdcFramebuffer, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

CStarObject::CStarObject(const RECT& rect) : CPolygonObject(rect)
{
	n_type = eType::Star;

	int nHalfWidth = int((rect.right - rect.left) * 0.5f);
	int nHalfHeight = int((rect.bottom - rect.top) * 0.5f);
	int x0 = -nHalfWidth, x1 = -int(nHalfWidth * (2.0f / 3.0f)), x2 = -int(nHalfWidth * (1.0f / 3.0f)), x3 = 0, x4 = int(nHalfWidth * (1.0f / 3.0f)), x5 = int(nHalfWidth * (2.0f / 3.0f)), x6 = nHalfWidth;
	int y0 = -nHalfHeight, y1 = -int(nHalfHeight * 0.5f), y2 = 0, y3 = int(nHalfWidth * 0.5f), y4 = nHalfWidth;
	SetVertices({ {x3,y0}, {x4,y1}, {x6,y1}, {x5,y2}, {x6,y3}, {x4,y3}, {x3,y4}, {x2,y3}, {x0,y3}, {x1,y2}, {x0,y1}, {x2,y1} });
}

CHexagonObject::CHexagonObject(const RECT& rect) : CPolygonObject(rect)
{
	n_type = eType::Hexagon;

	int nHalfWidth = int((rect.right - rect.left) * 0.5f);
	int nHalfHeight = int((rect.bottom - rect.top) * 0.5f);
	int x0 = -nHalfWidth, x1 = -int(nHalfWidth * 0.5f), x2 = int(nHalfWidth * 0.5f), x3 = nHalfWidth;
	int y0 = -nHalfHeight, y1 = 0, y2 = nHalfHeight;
	SetVertices({ {x0,y1}, {x1,y0}, {x2,y0}, {x3,y1}, {x2,y2}, {x1,y2} });
}