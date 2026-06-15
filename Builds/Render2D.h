#pragma once

#include "GraphicsTypes.h"

#include <GL/glew.h>

namespace ve::rendering
{
	struct TexturedQuadRequest
	{
		GLuint texture = 0;
		ScreenRect rect;
		ColorRgba tint;
	};

	class Canvas2D
	{
	public:
		/**
		 * Draws a screen-space textured quad from a request object.
		 *
		 * @param request Texture id, rectangle, and tint.
		 */
		void DrawTexturedQuad(const TexturedQuadRequest& request) const;

		/**
		 * Draws a filled screen-space triangle.
		 *
		 * @param triangle Triangle coordinates in pixels.
		 * @param color Fill color.
		 */
		void DrawSolidTriangle(const ScreenTriangle& triangle, const ColorRgba& color) const;

		/**
		 * Draws a filled screen-space rectangle.
		 *
		 * @param rect Rectangle coordinates in pixels.
		 * @param color Fill color.
		 */
		void DrawSolidRect(const ScreenRect& rect, const ColorRgba& color) const;

		/**
		 * Draws an outlined screen-space rectangle.
		 *
		 * @param rect Rectangle coordinates in pixels.
		 * @param color Outline color.
		 * @param line_width Width in pixels.
		 */
		void DrawRectOutline(const ScreenRect& rect, const ColorRgba& color, float line_width) const;
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
	 * Draws a filled screen-space triangle.
	 *
	 * @param triangle Triangle coordinates in pixels.
	 * @param color Fill color.
	 */
	void DrawSolidTriangle(const ScreenTriangle& triangle, const ColorRgba& color);

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
