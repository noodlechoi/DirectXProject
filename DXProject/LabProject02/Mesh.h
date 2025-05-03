#pragma once
class CPoint3D {
public:
	CPoint3D() {}
	CPoint3D(float X, float Y, float Z) { x = X; y = Y; z = Z; }
public:
	float x{};
	float y{};
	float z{};
};

class CVectex {
public:
	CVectex() {}
	CVectex(CPoint3D p) { position = p; }
public:
	CPoint3D position{};
};

class CPolygon {
public:
	CPolygon() {}
	CPolygon(int vertexNum);

	void SetVertex(CVectex);
public:
	std::vector<CVectex> vertexes{};
};

// Mesh - 공유 자원(com 객체처럼 작동)
class CMesh {
public:
	CMesh(int polygonNum);

	void AddRef() { ++ref_num; }
	void Release() { --ref_num; if (0 >= ref_num) delete this; }

	void SetPolygon(CPolygon);
	virtual void Render(HDC);
private:
	int ref_num{ 1 };

	std::vector<CPolygon> polygons;
};