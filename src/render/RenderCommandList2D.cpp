#include "RenderCommandList.h"

namespace ve::rendering
{
	void RenderCommandList::DrawTriangle(
		ScreenTriangle triangle, ColorRgba color, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, DrawTriangle2DCommand{ triangle, color } });
	}

	void RenderCommandList::DrawSolidRect(
		ScreenRect rect, ColorRgba color, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, DrawRect2DCommand{ rect, color, true, 1.0f } });
	}

	void RenderCommandList::DrawRectOutline(
		ScreenRect rect, ColorRgba color, float line_width, RenderSortKey sort_key)
	{
		commands_.push_back(
			RenderCommand{ sort_key, DrawRect2DCommand{ rect, color, false, line_width } });
	}

	void RenderCommandList::DrawTexturedQuad(
		TextureHandle texture, ScreenRect rect, ColorRgba tint, RenderSortKey sort_key)
	{
		commands_.push_back(
			RenderCommand{ sort_key, DrawTexturedQuad2DCommand{ texture, rect, tint } });
	}

	void RenderCommandList::DrawText(
		std::string text, glm::vec2 origin, float scale, ColorRgba color, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{
			sort_key,
			DrawText2DCommand{ std::pmr::string{ text, &command_memory_resource_ }, origin, scale, color }
		});
	}
}
