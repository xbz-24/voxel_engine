#include "Render2D.h"

namespace ve::rendering
{
	void DrawTexturedQuad(TextureHandle texture, float x, float y, float width, float height)
	{
		Canvas2D{}.DrawTexturedQuad(
			TexturedQuadRequest{ texture, ScreenRect{ x, y, width, height }, ColorRgba{} });
	}

	void DrawTexturedQuad(const TexturedQuadRequest& request)
	{
		Canvas2D{}.DrawTexturedQuad(request);
	}

	void DrawSolidTriangle(const ScreenTriangle& triangle, const ColorRgba& color)
	{
		Canvas2D{}.DrawSolidTriangle(triangle, color);
	}

	void DrawSolidRect(const ScreenRect& rect, const ColorRgba& color)
	{
		Canvas2D{}.DrawSolidRect(rect, color);
	}

	void DrawRectOutline(const ScreenRect& rect, const ColorRgba& color, float line_width)
	{
		Canvas2D{}.DrawRectOutline(rect, color, line_width);
	}
}
