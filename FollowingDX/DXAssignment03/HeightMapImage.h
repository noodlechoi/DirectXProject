#pragma once
class CHeightMapImage
{
public:
	CHeightMapImage(LPCTSTR, int, int, XMFLOAT3);

	// 선형 보간한 높이 값
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

