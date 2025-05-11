#pragma once

class CVertex {
public:
	CVertex() {position = XMFLOAT3(0.0f, 0.0f, 0.0f);}
	CVertex(float x, float y, float z) { position = XMFLOAT3(x, y, z); }

	CVertex(const CVertex&);
	CVertex& operator=(const CVertex&);
	CVertex(CVertex&&);
	CVertex& operator=(CVertex&&);

	virtual void Save(std::ostream&) const;
	virtual std::istream& Load(std::istream&);
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

	virtual void Save(std::ostream&) const;
	virtual std::istream& Load(std::istream&);
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
	int GetRefNum() const { return ref_num; }
	CPolygon* GetPolygon()  { return polygons.data(); };
	virtual void Render(HDC) const;

	BOOL RayIntersectionByTriangle(XMVECTOR& , XMVECTOR& , XMVECTOR , XMVECTOR , XMVECTOR , float* );
	int CheckRayIntersection(XMVECTOR& , XMVECTOR& , float* );

	virtual void Save(std::ostream&) const;
	virtual std::istream& Load(std::istream&);

	BoundingOrientedBox	OOBB{ BoundingOrientedBox() };
protected:
	int ref_num{ 1 };

	std::vector<CPolygon> polygons;
};

class CCubeMesh : public CMesh {
public:
	CCubeMesh(float = 4.0f, float = 4.0f, float = 4.0f);
	// 피벗도 같이 설정
	CCubeMesh(float width, float height, float depth, float x, float y, float z);

	void ChangePivot(float = 0.0f, float = 0.0f, float = 0.0f);
};

class CTextMesh : public CMesh {
public:
	CTextMesh(float = 24.0f, float = 8.0f, float = 4.0f, LPCTSTR = {L"3D 게임프로그래밍 1"});

	void Render(HDC) const override;
	LPCTSTR text{};
};

