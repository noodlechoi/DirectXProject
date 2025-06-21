#include "stdafx.h"
#include "HeightMapImage.h"

CHeightMapImage::CHeightMapImage(LPCTSTR fileName, int otherWidth, int otherLength, XMFLOAT3 otherScale) 
	: width{otherWidth}, length{otherLength}, scale{otherScale}, height_map_pixels{std::make_unique<BYTE[]>(width * length)}
{
	// 파일 읽기
	std::ifstream in{ fileName, std::ios::binary };
	if (!in)
		throw;

	BYTE* heightMapPixels = new BYTE[width * length];
	in.read((char*)heightMapPixels, width * length);

	// 좌표축 상하대칭 저장
	for (int y = 0; y < length; ++y) {
		for (int x = 0; x < width; ++x) {
			height_map_pixels[x + ((length - 1 - y) * width)] = heightMapPixels[x + y * width];
		}
	}

	if (heightMapPixels) delete[] heightMapPixels;
}

float CHeightMapImage::GetHeight(float x, float z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= width) || (z >= length)) {
		return 0.0f;
	}

	int intX{ (int)x };
	int intZ{ (int)z };
	float xPercent{ x - intX };
	float zPercent{ z - intZ};

	float bottomLeft{ (float)height_map_pixels[intX + (intZ * width)] };
	float bottomRight{ (float)height_map_pixels[(intX + 1)+(intZ * width)] };
	float topLeft{ (float)height_map_pixels[intX + ((intZ + 1)*width)] };
	float topRight{ (float)height_map_pixels[(intX + 1)+((intZ + 1) * width)] };
#ifdef  _WITH_APPROXIMATE_OPPOSITE_CORNER
	// z 좌표가 홀수인 경우 인덱스가 오른쪽에서 왼쪽으로 나열
	bool rightToLeft{ (z % 2 != 0) };
	// 같은 평면 상에서 보간이 되도록
	if (rightToLeft) {
		// 왼쪽 위 삼각형
		if (zPercent >= xPercent)
			bottomRight = bottomLeft + (topRight - topLeft);
		else // 오른쪽 아래 삼각형
			topLeft = topRight + (bottomLeft - bottomRight);
	} else {
		if (zPercent < 1.0f - xPercent)
			topRight = topLeft + (bottomRight - bottomLeft);
		else
			BottomLeft = topLeft + (bottomRight - topRight);
	}
#endif //  _WITH_APPROXIMATE_OPPOSITE_CORNER
	// 네 점 보간
	float topHeight{ topLeft * (1 - xPercent) + topRight * xPercent };
	float bottomHeight{ bottomLeft* (1 - xPercent) + bottomRight* xPercent };

	return (bottomHeight * (1 - zPercent) + topHeight * zPercent);
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	// x, y좌표가 높이 맵 범위를 벗어나면 법선 벡터는 y축 방향 벡터
	if ((x < 0.0f) || (z < 0.0f) || (x >= width) || (z >= length)) {
		return XMFLOAT3(0.0f, 1.0f, 0.0f);
	}

	// 높이 맵에서 인접한 정점에 대한 픽셀 값을 이용해 법선 벡터 계산
	int heightMapIndex{ x + (z * width) };
	int xHeightMapAdd{ (x < (width - 1)) ? 1 : -1 };
	int yHeightMapAdd {(z <(length - 1)) ? width : -width};
	// 픽셀 지형의 높이를 구한다
	float y1{ (float)height_map_pixels[heightMapIndex] * scale.y };
	float y2{ (float)height_map_pixels[heightMapIndex + xHeightMapAdd] * scale.y };
	float y3{ (float)height_map_pixels[heightMapIndex + yHeightMapAdd] * scale.y };
	// Edge1, Edge2 => y1을 기준으로 삼각형 사이 벡터
	XMFLOAT3 edge1{ XMFLOAT3(0.0f, y3 - y1, scale.z) };
	XMFLOAT3 edge2{ XMFLOAT3(scale.x, y2 - y1, 0) };
	// 법선 벡터
	XMFLOAT3 normal{ Vector3::CrossProduct(edge1, edge2, true) };

	return normal;

}
