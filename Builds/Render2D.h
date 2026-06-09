#pragma once

#include <GL/glew.h>

namespace ve::rendering
{
	struct ColorRgba
	{
		float red = 1.0f;
		float green = 1.0f;
		float blue = 1.0f;
		float alpha = 1.0f;
	};

	struct ScreenRect
	{
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
	};

	struct TexturedQuadRequest
	{
		GLuint texture = 0;
		ScreenRect rect;
		ColorRgba tint;
	};

	/**
	 * Draws a screen-space textured quad using the active projection/modelview.
	 *
	 * @param texture OpenGL texture id.
	 * @param x Left screen coordinate.
	 * @param y Top screen coordinate.
	 * @param width Quad width in pixels.
	 * @param height Quad height in pixels.
	 */
	void DrawTexturedQuad(GLuint texture, float x, float y, float width, float height);

	/**
	 * Draws a screen-space textured quad from a request object.
	 *
	 * @param request Texture id, rectangle, and tint.
	 */
	void DrawTexturedQuad(const TexturedQuadRequest& request);

	/**
	 * Draws a filled screen-space rectangle.
	 *
	 * @param rect Rectangle coordinates in pixels.
	 * @param color Fill color.
	 */
	void DrawSolidRect(const ScreenRect& rect, const ColorRgba& color);

	/**
	 * Draws an outlined screen-space rectangle.
	 *
	 * @param rect Rectangle coordinates in pixels.
	 * @param color Outline color.
	 * @param line_width Width in pixels.
	 */
	void DrawRectOutline(const ScreenRect& rect, const ColorRgba& color, float line_width);
}
