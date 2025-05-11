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
	// �̵� ������
	CPolygon( CPolygon&&);
	// �̵� �Ҵ� ������
	CPolygon& operator=( CPolygon&&);

	void SetVertex(CVertex);

	virtual void Save(std::ostream&) const;
	virtual std::istream& Load(std::istream&);
public:
	std::vector<CVertex> vertexes;
};

// Mesh - ���� �ڿ�(com ��üó�� �۵�)
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
	// �ǹ��� ���� ����
	CCubeMesh(float width, float height, float depth, float x, float y, float z);

	void ChangePivot(float = 0.0f, float = 0.0f, float = 0.0f);
};

class CTextMesh : public CMesh {
public:
	CTextMesh(float = 24.0f, float = 8.0f, float = 4.0f, LPCTSTR = {L"3D �������α׷��� 1"});

	void Render(HDC) const override;
	LPCTSTR text{};
};

