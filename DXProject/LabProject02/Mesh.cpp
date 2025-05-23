#include "stdafx.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"

CVertex::CVertex(const CVertex& rhs) : position{rhs.position}
{

}

CVertex& CVertex::operator=(const CVertex& rhs)
{
	if (this == &rhs) return *this;
	position = rhs.position;
	
	return *this;
}

CVertex::CVertex(CVertex&& other) : position{ std::move(other.position) }
{
}

CVertex& CVertex::operator=(CVertex&& other)
{
	if (this == &other) return *this;
	position = std::move(other.position);

	return *this;
}

CPolygon::CPolygon(int vertexNum)
{
	vertexes.reserve(vertexNum);
}

// 나중에 리펙토링
CPolygon::CPolygon(CVertex v1, CVertex v2, CVertex v3, CVertex v4)
{
	vertexes.reserve(3);
	vertexes.push_back(v1);
	vertexes.push_back(v2);
	vertexes.push_back(v3);
	vertexes.push_back(v4);
}

CPolygon::CPolygon(const CPolygon& other)
{
	vertexes.reserve(other.vertexes.size());
	vertexes = other.vertexes;
}

CPolygon& CPolygon::operator=(const CPolygon& other)
{
	if (this == &other)
		return *this;

	if (vertexes.size() != other.vertexes.size()) {
		throw;
	}

	vertexes.reserve(other.vertexes.size());
	vertexes = other.vertexes;
	return *this;
}

CPolygon::CPolygon(const CPolygon&& other) : vertexes{ std::move(other.vertexes) }
{
}

CPolygon& CPolygon::operator=(const CPolygon&& other)
{
	if (this == &other)
		return *this;

	vertexes = std::move(other.vertexes);
	return *this;
}


void CPolygon::SetVertex(CVertex vertex)
{
	vertexes.push_back(vertex);
}

CMesh::CMesh(int polygonNum)
{
	polygons.reserve(polygonNum);
}

//void CMesh::SetPolygon(CPolygon polygon)
//{
//	polygons.push_back(polygon);
//}

void CMesh::SetPolygon(CPolygon&& polygon)
{
	polygons.push_back(polygon);
}

void Draw2DLine(HDC hDCFrameBuffer, XMFLOAT3& previousProject, XMFLOAT3& currentProject)
{
	XMFLOAT3 previous = CGraphicsPipeline::ScreenTransform(previousProject);
	XMFLOAT3 current = CGraphicsPipeline::ScreenTransform(currentProject);
	::MoveToEx(hDCFrameBuffer, (long)previous.x, (long)previous.y, NULL);
	::LineTo(hDCFrameBuffer, (long)current.x, (long)current.y);
}

void CMesh::Render(HDC hDCFrameBuffer) const
{
	XMFLOAT3 initialProject, previousProject;
	// 보이는지 확인
	bool isPreviousInside{ false }, isInitialInside{ false }, isCurrentInside{ false }, isIntersectInside{ false };

	for (const auto& polygon : polygons) {
		initialProject = previousProject = CGraphicsPipeline::Project(polygon.vertexes[0].position);
		isPreviousInside = isInitialInside = (-1.0f <= initialProject.x) && (initialProject.x <= 1.0f) && (-1.0f <= initialProject.y) && (initialProject.y <= 1.0f);
		for (const auto& v : polygon.vertexes) {
			XMFLOAT3 currentProject = CGraphicsPipeline::Project(v.position);
			isCurrentInside = (-1.0f <= currentProject.x) && (currentProject.x <= 1.0f) && (-1.0f <= currentProject.y) && (currentProject.y <= 1.0f);

			if (((0.0f <= currentProject.z) && (currentProject.z <= 1.0f)) && ((isCurrentInside || isPreviousInside))) ::Draw2DLine(hDCFrameBuffer, previousProject, currentProject);
			previousProject = currentProject;
			isPreviousInside = isCurrentInside;
		}
		// 마지막 점과 첫번 째 점 연결
		if (((0.0f <= initialProject.z) && (initialProject.z <= 1.0f)) && ((isInitialInside || isPreviousInside))) ::Draw2DLine(hDCFrameBuffer, previousProject, initialProject);
	}
}


CCubeMesh::CCubeMesh(float width, float height, float depth) : CMesh(6)
{
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;
	float halfDepth = depth * 0.5f;

	polygons.push_back(CPolygon(CVertex(-halfWidth, +halfHeight, -halfDepth),
		CVertex(+halfWidth, +halfHeight, -halfDepth),
		CVertex(+halfWidth, -halfHeight, -halfDepth),
		CVertex(-halfWidth, -halfHeight, -halfDepth)
		));

	polygons.push_back(CPolygon(CVertex(-halfWidth, +halfHeight, +halfDepth),
		CVertex(+halfWidth, +halfHeight, +halfDepth),
		CVertex(+halfWidth, +halfHeight, -halfDepth),
		CVertex(-halfWidth, +halfHeight, -halfDepth)
	));

	polygons.push_back(CPolygon(CVertex(-halfWidth, -halfHeight, +halfDepth),
		CVertex(+halfWidth, -halfHeight, +halfDepth),
		CVertex(+halfWidth, +halfHeight, +halfDepth),
		CVertex(-halfWidth, +halfHeight, +halfDepth)
	));

	polygons.push_back(CPolygon(CVertex(-halfWidth, -halfHeight, -halfDepth),
		CVertex(+halfWidth, -halfHeight, -halfDepth),
		CVertex(+halfWidth, -halfHeight, +halfDepth),
		CVertex(-halfWidth, -halfHeight, +halfDepth)
	));

	polygons.push_back(CPolygon(CVertex(-halfWidth, +halfHeight, +halfDepth),
		CVertex(-halfWidth, +halfHeight, -halfDepth),
		CVertex(-halfWidth, -halfHeight, -halfDepth),
		CVertex(-halfWidth, -halfHeight, +halfDepth)
	));

	polygons.push_back(CPolygon(CVertex(+halfWidth, +halfHeight, -halfDepth),
		CVertex(+halfWidth, +halfHeight, +halfDepth),
		CVertex(+halfWidth, -halfHeight, +halfDepth),
		CVertex(+halfWidth, -halfHeight, -halfDepth)
	));


}
