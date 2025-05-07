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

void CVertex::Save(std::ostream& out) const
{
	out.write(reinterpret_cast<const char*>(&position), sizeof(position));
}

std::istream& CVertex::Load(std::istream& in)
{
	in.read(reinterpret_cast<char*>(&position), sizeof(position));
	return in;
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

CPolygon::CPolygon( CPolygon&& other) : vertexes{ std::move(other.vertexes) }
{
}

CPolygon& CPolygon::operator=( CPolygon&& other)
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

void CPolygon::Save(std::ostream& out) const
{
	int cnt{};
	size_t vertexCount = vertexes.size();
	out.write(reinterpret_cast<const char*>(&vertexCount), sizeof(size_t));
	for (const auto& v : vertexes) {
		v.Save(out);
		++cnt;
	}

	std::wstring debugMessage = L"CPolygon Save cnt: " + std::to_wstring(cnt) + L"\n" + L"size: " + std::to_wstring(vertexCount) + L"\n";
	OutputDebugString(debugMessage.c_str());
}

std::istream& CPolygon::Load(std::istream& in)
{
	size_t vertexCount;
	in.read(reinterpret_cast<char*>(&vertexCount), sizeof(size_t));
	vertexes.reserve(vertexCount);
	for (int i = 0; i < vertexCount; ++i) {
		CVertex v;
		v.Load(in);
		vertexes.push_back(std::move(v));
	}

	return in;
}

CMesh::CMesh(int polygonNum)
{
	polygons.reserve(polygonNum);
}

CMesh::CMesh(const CMesh& other) :
	ref_num{ other.ref_num },
	polygons{ other.polygons }
{
}

CMesh& CMesh::operator=(const CMesh& other)
{
	if (this == &other) return *this;

	ref_num = other.ref_num;
	polygons = other.polygons;

	return *this;
}

CMesh::CMesh(CMesh&& other) :
	ref_num{ other.ref_num },
	polygons{ std::move(other.polygons) }
{
}

CMesh& CMesh::operator=(CMesh&& other)
{
	if (this == &other) return *this;

	ref_num = other.ref_num;
	polygons = std::move(other.polygons);

	return *this;
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

void CMesh::Save(std::ostream& out) const
{
	int cnt{};
	size_t polygonCount = polygons.size();
	out.write(reinterpret_cast<const char*>(&polygonCount), sizeof(size_t));
	for (const auto& polygon : polygons) {
		polygon.Save(out);
		++cnt;
	}

	std::wstring debugMessage = L"CMesh Save cnt: " + std::to_wstring(cnt) + L"\n" + L"size: " + std::to_wstring(polygonCount) + L"\n";
	OutputDebugString(debugMessage.c_str());
}

std::istream& CMesh::Load(std::istream& in)
{
	size_t polygonCount;
	in.read(reinterpret_cast<char*>(&polygonCount), sizeof(size_t));
	polygons.reserve(polygonCount);
	for (int i = 0; i < polygonCount; ++i) {
		CPolygon polygon;
		polygon.Load(in);
		polygons.push_back(std::move(polygon));
	}

	return in;
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

CTextMesh::CTextMesh(float width, float height, float depth) :CMesh(6)
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

void CTextMesh::Render(HDC hDCFrameBuffer) const
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
