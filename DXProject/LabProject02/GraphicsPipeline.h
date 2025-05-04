#pragma once
#include "Camera.h"

class CGraphicsPipeline{
public:
	static void SetWorldTransform(XMFLOAT4X4& pxmf4x4World) { world_matrix = pxmf4x4World; }
	static void SetViewPerspectiveProjectTransform(XMFLOAT4X4);
	static void SetViewport(CViewport& viewPort) { viewport = viewPort; }

	static XMFLOAT3 ScreenTransform(XMFLOAT3& );
	static XMFLOAT3 Project(const XMFLOAT3& );
private:
	CGraphicsPipeline() = delete;

	static XMFLOAT4X4 world_matrix;
	static XMFLOAT4X4 view_project_matrix;
	static CViewport viewport;
};

