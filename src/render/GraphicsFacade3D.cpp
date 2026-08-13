#include "GraphicsFacade.h"

namespace ve::rendering
{
	void GraphicsFacade::DrawCube(
		glm::vec3 center, glm::vec3 size, ColorRgba color,
		RenderSortKey sort_key, GraphicsCommandLayer layer)
	{
		Commands(layer).DrawSolidCube(center, size, color, sort_key);
	}

	void GraphicsFacade::DrawCubeOutline(
		glm::vec3 center, glm::vec3 size, ColorRgba color, float line_width,
		RenderSortKey sort_key, GraphicsCommandLayer layer)
	{
		Commands(layer).DrawWireCube(center, size, color, line_width, sort_key);
	}

	void GraphicsFacade::SetScissorRect(
		ScreenRect rect, RenderSortKey sort_key, GraphicsCommandLayer layer)
	{
		Commands(layer).SetScissorRect(rect, sort_key);
	}

	void GraphicsFacade::ClearScissorRect(
		RenderSortKey sort_key, GraphicsCommandLayer layer)
	{
		Commands(layer).ClearScissorRect(sort_key);
	}
}
