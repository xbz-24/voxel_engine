#include "Render2D.h"

#include "TextureLoader.h"

namespace ve::rendering
{
	namespace
	{
		/// Applies an RGBA color to the fixed-function OpenGL pipeline.
		void ApplyColor(const ColorRgba& color)
		{
			glColor4f(color.red, color.green, color.blue, color.alpha);
		}

		/// Emits one axis-aligned rectangle without texture coordinates.
		void EmitRectVertices(const ScreenRect& rect)
		{
			glVertex2f(rect.x, rect.y);
			glVertex2f(rect.x + rect.width, rect.y);
			glVertex2f(rect.x + rect.width, rect.y + rect.height);
			glVertex2f(rect.x, rect.y + rect.height);
		}

		/// Emits one screen-space triangle without texture coordinates.
		void EmitTriangleVertices(const ScreenTriangle& triangle)
		{
			glVertex2f(triangle.first.x, triangle.first.y);
			glVertex2f(triangle.second.x, triangle.second.y);
			glVertex2f(triangle.third.x, triangle.third.y);
		}
	}

	void Canvas2D::DrawTexturedQuad(const TexturedQuadRequest& request) const
	{
		glBindTexture(GL_TEXTURE_2D, NativeOpenGLTexture(request.texture));
		ApplyColor(request.tint);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(request.rect.x, request.rect.y);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(request.rect.x + request.rect.width, request.rect.y);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(request.rect.x + request.rect.width, request.rect.y + request.rect.height);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(request.rect.x, request.rect.y + request.rect.height);
		glEnd();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void Canvas2D::DrawSolidTriangle(const ScreenTriangle& triangle, const ColorRgba& color) const
	{
		glDisable(GL_TEXTURE_2D);
		ApplyColor(color);
		glBegin(GL_TRIANGLES);
		EmitTriangleVertices(triangle);
		glEnd();
		glEnable(GL_TEXTURE_2D);
	}

	void Canvas2D::DrawSolidRect(const ScreenRect& rect, const ColorRgba& color) const
	{
		glDisable(GL_TEXTURE_2D);
		ApplyColor(color);
		glBegin(GL_QUADS);
		EmitRectVertices(rect);
		glEnd();
		glEnable(GL_TEXTURE_2D);
	}

	void Canvas2D::DrawRectOutline(const ScreenRect& rect, const ColorRgba& color, float line_width) const
	{
		glDisable(GL_TEXTURE_2D);
		glLineWidth(line_width);
		ApplyColor(color);
		glBegin(GL_LINE_LOOP);
		EmitRectVertices(rect);
		glEnd();
		glLineWidth(1.0f);
		glEnable(GL_TEXTURE_2D);
	}

	void DrawTexturedQuad(TextureHandle texture, float x, float y, float width, float height)
	{
		Canvas2D{}.DrawTexturedQuad(TexturedQuadRequest{ texture, ScreenRect{ x, y, width, height }, ColorRgba{} });
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
