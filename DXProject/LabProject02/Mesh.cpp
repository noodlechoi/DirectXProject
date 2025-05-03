#include "stdafx.h"
#include "Mesh.h"

CPolygon::CPolygon(int vertexNum)
{
	vertexes.reserve(vertexNum);
}

void CPolygon::SetVertex(CVectex vertex)
{
	vertexes.push_back(vertex);
}

CMesh::CMesh(int polygonNum)
{
	polygons.reserve(polygonNum);
}

void CMesh::SetPolygon(CPolygon polygon)
{
	polygons.push_back(polygon);
}

void CMesh::Render(HDC hDCFrameBuffer)
{

}
