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
	
	// 삼각형 면 광선 교차 확인
	BOOL RayIntersectionByTriangle(XMVECTOR&, XMVECTOR&, XMVECTOR, XMVECTOR, XMVECTOR, float&);
	// 교차된 면 개수 반환. 모델좌표계로 변환한 광선을 알고 있음
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

