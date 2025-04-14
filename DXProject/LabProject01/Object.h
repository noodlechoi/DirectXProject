#pragma once
#include "stdafx.h"

typedef struct Point {
	int x, y;
}Point;

typedef struct Size {
	int width, height;
}Size;

class CObject {
public:
	enum class eType {
		Rectangle,
		RoundRectangle,
		Ellipse,
		Star,
		Hexagon
	};

	void SetPosition(Point position) { pos = position; }
	void SetType(eType type) { n_type = type; }
	void SetColor(DWORD color) { dw_color = color; }
	eType GetType() { return n_type; }
	
	virtual void Animate(float fElapsedTime) {};
	virtual void Render(HDC) = 0;
protected:
	eType n_type;
	DWORD dw_color{ RGB(255,0,0) };

	Point pos{};

};

class CRectangleObject : public CObject {
public:
	CRectangleObject(const RECT& rect);

	void Render(HDC) override;
protected:
	Size size{};
};

class CRoundRectangleObject : public CRectangleObject {
public:
	CRoundRectangleObject(const RECT& rect);

	void Render(HDC) override;
};

class CEllipseObject : public CObject {
public:
	CEllipseObject(const RECT& rect);

	void Render(HDC) override;
private:
	Size size{};
};

class CPolygonObject : public CObject {
public:
	CPolygonObject(const RECT& rect);
	
	void SetVertices(const std::vector<POINT>& pVertices);
	void Render(HDC) override;
protected:
	std::vector<POINT> p_vertices{};
};

class CStarObject : public CPolygonObject {
public:
	CStarObject(const RECT& rect);
};

class CHexagonObject : public CPolygonObject {
public:
	CHexagonObject(const RECT& rect);
};