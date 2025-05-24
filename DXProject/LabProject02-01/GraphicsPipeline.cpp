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

XMFLOAT3 CGraphicsPipeline::Project(XMFLOAT3& model)
{
	XMFLOAT4X4 transformMatrix = Matrix4x4::Multiply(world_matrix, view_project_matrix);
	XMFLOAT3 project = Vector3::TransformCoord(model, transformMatrix);

	return project;
}

XMFLOAT3 CGraphicsPipeline::Transform(XMFLOAT3& model)
{
	XMFLOAT3 project = Project(model);
	XMFLOAT3 screen = ScreenTransform(project);

	return screen;
}
