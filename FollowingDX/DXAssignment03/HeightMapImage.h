#pragma once
class CHeightMapImage
{
public:
	CHeightMapImage(LPCTSTR, int, int, XMFLOAT3);

	// ���� ������ ���� ��
	float GetHeight(float, float);
	XMFLOAT3 GetHeightMapNormal(int, int);

	XMFLOAT3 GetScale() { return scale; }
	BYTE* GetHeightMapPixels() { return height_map_pixels.get(); }
	int GetWidth() { return width; }
	int GetLength() { return length; }
private:
	int width, length;
	XMFLOAT3 scale;
	std::unique_ptr<BYTE[]> height_map_pixels;
};

