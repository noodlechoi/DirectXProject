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

// ���߿� �����丵
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
	// ���̴��� Ȯ��
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
		// ������ ���� ù�� ° �� ����
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

CTextMesh::CTextMesh(float width, float height, float depth) : CMesh(100) // ������ ������ �뷫���� ��
{
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;
	float halfDepth = depth * 0.5f;

	float charWidth = width / 10.0f; // �� ������ �ʺ�
	float charHeight = height / 10.0f; // �� ������ ����
	float spacing = charWidth * 0.2f; // ���� �� ����
	float fontSizeX{ halfWidth * 0.2f };// ���� ũ��
	float fontSizeY{ halfHeight * 0.5f };// ���� ũ��
	float innerfontX{ fontSizeX * 0.5f };// ���� ���� ũ��
	float innerfontY{ fontSizeY * 0.2f };// ���� ���� ũ��
	float startX = -halfWidth; // �ؽ�Ʈ ���� ��ġ
	float startY = halfHeight; // �ؽ�Ʈ ���� ����


	// "3" ���� ����
	// -
	polygons.push_back(CPolygon(CVertex(startX, startY, -halfDepth),
		CVertex(startX + innerfontX, startY, -halfDepth),
		CVertex(startX + innerfontX, startY - innerfontY, -halfDepth),
		CVertex(startX, startY - innerfontY, -halfDepth)
	));
	// -
	polygons.push_back(CPolygon(CVertex(startX, startY - (innerfontY * 2), -halfDepth),
		CVertex(startX + innerfontX, startY - (innerfontY * 2), -halfDepth),
		CVertex(startX + innerfontX, startY - (innerfontY * 3), -halfDepth),
		CVertex(startX, startY - (innerfontY * 3), -halfDepth)
	));
	// -
	polygons.push_back(CPolygon(CVertex(startX, startY - (innerfontY * 4), -halfDepth),
		CVertex(startX + innerfontX, startY - (innerfontY * 4), -halfDepth),
		CVertex(startX + innerfontX, startY - (innerfontY * 5), -halfDepth),
		CVertex(startX, startY - (innerfontY * 5), -halfDepth)
	));
	// ��
	polygons.push_back(CPolygon(CVertex(startX + innerfontX, startY, +halfDepth),
		CVertex(startX + fontSizeX, startY, -halfDepth),
		CVertex(startX + fontSizeX, startY - fontSizeY, -halfDepth),
		CVertex(startX + innerfontX, startY - fontSizeY, -halfDepth)
	));


	// ��
	polygons.push_back(CPolygon(CVertex(startX + innerfontX, startY, +halfDepth),
		CVertex(startX + fontSizeX, startY, +halfDepth),
		CVertex(startX + fontSizeX, startY - fontSizeY, +halfDepth),
		CVertex(startX + innerfontX, startY - fontSizeY, +halfDepth)
	));
	// ��
	polygons.push_back(CPolygon(CVertex(startX, startY, +halfDepth),
		CVertex(startX + innerfontX, startY, +halfDepth),
		CVertex(startX + innerfontX, startY - innerfontY, +halfDepth),
		CVertex(startX, startY - innerfontY, +halfDepth)
	));
	// -
	polygons.push_back(CPolygon(CVertex(startX, startY - (innerfontY * 2), +halfDepth),
		CVertex(startX + innerfontX, startY - (innerfontY * 2), +halfDepth),
		CVertex(startX + innerfontX, startY - (innerfontY * 3), +halfDepth),
		CVertex(startX, startY - (innerfontY * 3), +halfDepth)
	));
	// -
	polygons.push_back(CPolygon(CVertex(startX, startY - (innerfontY * 4), +halfDepth),
		CVertex(startX + innerfontX, startY - (innerfontY * 4), +halfDepth),
		CVertex(startX + innerfontX, startY - (innerfontY * 5), +halfDepth),
		CVertex(startX, startY - (innerfontY * 5), +halfDepth)
	));


	// "D" ���� ����
	

}