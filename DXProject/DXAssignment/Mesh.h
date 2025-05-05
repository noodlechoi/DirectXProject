#pragma once

class CVertex {
public:
	CVertex() {position = XMFLOAT3(0.0f, 0.0f, 0.0f);}
	CVertex(float x, float y, float z) { position = XMFLOAT3(x, y, z); }

	CVertex(const CVertex&);
	CVertex& operator=(const CVertex&);
	CVertex(CVertex&&);
	CVertex& operator=(CVertex&&);
public:
	XMFLOAT3 position{};
};

class CPolygon {
public:
	CPolygon() {}
	CPolygon(int vertexNum);
	CPolygon(CVertex, CVertex, CVertex, CVertex);
	CPolygon(const CPolygon&);
	CPolygon& operator=(const CPolygon&);
	// 이동 생성자
	CPolygon( CPolygon&&);
	// 이동 할당 연산자
	CPolygon& operator=( CPolygon&&);

	void SetVertex(CVertex);
public:
	std::vector<CVertex> vertexes;
};

// Mesh - 공유 자원(com 객체처럼 작동)
class CMesh {
public:
	CMesh(int = 6);
	CMesh(const CMesh&);
	CMesh& operator=(const CMesh&);
	CMesh(CMesh&&);
	CMesh& operator=(CMesh&&);

	void AddRef() { ++ref_num; }
	void Release() { --ref_num; if (0 >= ref_num) delete this; }

	//void SetPolygon(CPolygon);
	void SetPolygon(CPolygon&&);
	virtual void Render(HDC) const;
protected:
	int ref_num{ 1 };

	std::vector<CPolygon> polygons;
};

class CCubeMesh : public CMesh {
public:
	CCubeMesh(float = 4.0f, float = 4.0f, float = 4.0f);
};

class CTextMesh : public CMesh {
public:
	CTextMesh(float = 24.0f, float = 8.0f, float = 4.0f);

	void Render(HDC) const override;
	LPCTSTR text{L"3D 게임프로그래밍 1"};
};

