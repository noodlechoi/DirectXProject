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
	CPolygon(const CPolygon&&);
	CPolygon& operator=(const CPolygon&&);

	void SetVertex(CVertex);
	size_t GetVertexSize() const { return vertexes.size(); }
public:
	std::vector<CVertex> vertexes;
};

class CMesh {
public:
	CMesh(int polygonNum);

	void SetPolygon(CPolygon);
	void SetPolygon(CPolygon&&);

	virtual void Render(HDC);
	
	// �ﰢ�� �� ���� ���� Ȯ��
	BOOL RayIntersectionByTriangle(XMVECTOR&, XMVECTOR&, XMVECTOR, XMVECTOR, XMVECTOR, float&);
	// ������ �� ���� ��ȯ. ����ǥ��� ��ȯ�� ������ �˰� ����
	size_t CheckRayIntersection(XMVECTOR&, XMVECTOR&, float&);

	BoundingBox AABB{ BoundingBox() };
protected:
	std::vector<CPolygon> polygons;
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(float = 4.0f, float = 4.0f, float = 4.0f);
};

