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
	};

	void SetPosition(Point position) { pos = position; }
	eType GetType() { return n_type; }
	void SetType(eType type) { n_type = type; }
	
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
