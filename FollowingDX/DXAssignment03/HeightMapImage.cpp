#include "stdafx.h"
#include "HeightMapImage.h"

CHeightMapImage::CHeightMapImage(LPCTSTR fileName, int otherWidth, int otherLength, XMFLOAT3 otherScale) 
	: width{otherWidth}, length{otherLength}, scale{otherScale}, height_map_pixels{std::make_unique<BYTE[]>(width * length)}
{
	// ���� �б�
	std::ifstream in{ fileName, std::ios::binary };
	if (!in)
		throw;

	BYTE* heightMapPixels = new BYTE[width * length];
	in.read((char*)heightMapPixels, width * length);

	// ��ǥ�� ���ϴ�Ī ����
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
	// z ��ǥ�� Ȧ���� ��� �ε����� �����ʿ��� �������� ����
	bool rightToLeft{ (z % 2 != 0) };
	// ���� ��� �󿡼� ������ �ǵ���
	if (rightToLeft) {
		// ���� �� �ﰢ��
		if (zPercent >= xPercent)
			bottomRight = bottomLeft + (topRight - topLeft);
		else // ������ �Ʒ� �ﰢ��
			topLeft = topRight + (bottomLeft - bottomRight);
	} else {
		if (zPercent < 1.0f - xPercent)
			topRight = topLeft + (bottomRight - bottomLeft);
		else
			BottomLeft = topLeft + (bottomRight - topRight);
	}
#endif //  _WITH_APPROXIMATE_OPPOSITE_CORNER
	// �� �� ����
	float topHeight{ topLeft * (1 - xPercent) + topRight * xPercent };
	float bottomHeight{ bottomLeft* (1 - xPercent) + bottomRight* xPercent };

	return (bottomHeight * (1 - zPercent) + topHeight * zPercent);
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	// x, y��ǥ�� ���� �� ������ ����� ���� ���ʹ� y�� ���� ����
	if ((x < 0.0f) || (z < 0.0f) || (x >= width) || (z >= length)) {
		return XMFLOAT3(0.0f, 1.0f, 0.0f);
	}

	// ���� �ʿ��� ������ ������ ���� �ȼ� ���� �̿��� ���� ���� ���
	int heightMapIndex{ x + (z * width) };
	int xHeightMapAdd{ (x < (width - 1)) ? 1 : -1 };
	int yHeightMapAdd {(z <(length - 1)) ? width : -width};
	// �ȼ� ������ ���̸� ���Ѵ�
	float y1{ (float)height_map_pixels[heightMapIndex] * scale.y };
	float y2{ (float)height_map_pixels[heightMapIndex + xHeightMapAdd] * scale.y };
	float y3{ (float)height_map_pixels[heightMapIndex + yHeightMapAdd] * scale.y };
	// Edge1, Edge2 => y1�� �������� �ﰢ�� ���� ����
	XMFLOAT3 edge1{ XMFLOAT3(0.0f, y3 - y1, scale.z) };
	XMFLOAT3 edge2{ XMFLOAT3(scale.x, y2 - y1, 0) };
	// ���� ����
	XMFLOAT3 normal{ Vector3::CrossProduct(edge1, edge2, true) };

	return normal;

}
