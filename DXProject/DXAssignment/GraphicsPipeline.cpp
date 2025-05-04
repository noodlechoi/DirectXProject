#include "stdafx.h"
#include "GraphicsPipeline.h"

XMFLOAT4X4 CGraphicsPipeline::world_matrix{};
XMFLOAT4X4 CGraphicsPipeline::view_project_matrix{ };
CViewport CGraphicsPipeline::viewport{ };


void CGraphicsPipeline::SetViewPerspectiveProjectTransform(XMFLOAT4X4 viewPerspectiveProject)
{
	view_project_matrix = viewPerspectiveProject;
}

XMFLOAT3 CGraphicsPipeline::ScreenTransform(XMFLOAT3& project)
{
	XMFLOAT3 screen = project;

	float halfWidth = viewport.width * 0.5f;
	float halfHeight = viewport.height * 0.5f;
	screen.x = viewport.left + (project.x * halfWidth) + halfWidth;
	screen.y = viewport.top + (-project.y * halfHeight) + halfHeight;

	return screen;
}

XMFLOAT3 CGraphicsPipeline::Project(const XMFLOAT3& model)
{
	XMMATRIX modelToProject = XMMatrixMultiply(XMLoadFloat4x4(&world_matrix), XMLoadFloat4x4(&view_project_matrix));
	XMFLOAT3 project;
	XMStoreFloat3(&project, XMVector3TransformCoord(XMLoadFloat3(&model), modelToProject));

	return project;
}
