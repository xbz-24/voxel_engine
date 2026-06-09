#include "Render2D.h"

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
	}

	void DrawTexturedQuad(GLuint texture, float x, float y, float width, float height)
	{
		DrawTexturedQuad(TexturedQuadRequest{ texture, ScreenRect{ x, y, width, height }, ColorRgba{} });
	}

	void DrawTexturedQuad(const TexturedQuadRequest& request)
	{
		glBindTexture(GL_TEXTURE_2D, request.texture);
		ApplyColor(request.tint);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(request.rect.x, request.rect.y);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(request.rect.x + request.rect.width, request.rect.y);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(request.rect.x + request.rect.width, request.rect.y + request.rect.height);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(request.rect.x, request.rect.y + request.rect.height);
		glEnd();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void DrawSolidRect(const ScreenRect& rect, const ColorRgba& color)
	{
		glDisable(GL_TEXTURE_2D);
		ApplyColor(color);
		glBegin(GL_QUADS);
		EmitRectVertices(rect);
		glEnd();
		glEnable(GL_TEXTURE_2D);
	}

	void DrawRectOutline(const ScreenRect& rect, const ColorRgba& color, float line_width)
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
}
