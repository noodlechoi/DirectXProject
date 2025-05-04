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
	CPolygon(const CPolygon&&);
	// 이동 할당 연산자
	CPolygon& operator=(const CPolygon&&);

	void SetVertex(CVertex);
public:
	std::vector<CVertex> vertexes;
};

// Mesh - 공유 자원(com 객체처럼 작동)
class CMesh {
public:
	CMesh(int polygonNum);

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

#include <string>
#include <vector>
#include "Mesh.h"

class CTextMesh : public CMesh {
public:
	CTextMesh(float = 4.0f, float = 4.0f, float = 4.0f);

	void SetText(const std::string& text) {
		polygons.clear(); // 기존 폴리곤 초기화
		for (size_t i = 0; i < text.size(); ++i) {
			AddCharacterPolygons(text[i], i);
		}
	}

private:
	void AddCharacterPolygons(char character, size_t index) {
		// 문자 위치 계산
		float offsetX = index * (charWidth + spacing);

		// 간단한 사각형 폴리곤으로 문자 표현 (예: 'A'를 사각형으로 표현)
		polygons.push_back(CPolygon(
			CVertex(offsetX, charHeight, 0.0f),
			CVertex(offsetX + charWidth, charHeight, 0.0f),
			CVertex(offsetX + charWidth, 0.0f, 0.0f),
			CVertex(offsetX, 0.0f, 0.0f)
		));
	}

	float charWidth;
	float charHeight;
	float charDepth;
	float spacing = 2.0f; // 문자 간 간격
};
