#include "RenderPrimitives3D.h"

#include <GL/glew.h>
#include <array>
#include <cstddef>

namespace
{
	/// Applies an RGBA color to the fixed-function pipeline.
	void ApplyColor(const ve::rendering::ColorRgba& color)
	{
		glColor4f(color.red, color.green, color.blue, color.alpha);
	}

	/// Builds the eight corners for an axis-aligned cube.
	std::array<glm::vec3, 8> BuildCubeCorners(const ve::rendering::CubeRequest& request)
	{
		const glm::vec3 half_size = request.size * 0.5f;
		const glm::vec3 min_corner = request.center - half_size;
		const glm::vec3 max_corner = request.center + half_size;
		return {{
			{ min_corner.x, min_corner.y, min_corner.z },
			{ max_corner.x, min_corner.y, min_corner.z },
			{ max_corner.x, max_corner.y, min_corner.z },
			{ min_corner.x, max_corner.y, min_corner.z },
			{ min_corner.x, min_corner.y, max_corner.z },
			{ max_corner.x, min_corner.y, max_corner.z },
			{ max_corner.x, max_corner.y, max_corner.z },
			{ min_corner.x, max_corner.y, max_corner.z }
		}};
	}

	/// Emits one vertex from a corner array.
	void EmitCorner(const std::array<glm::vec3, 8>& corners, std::size_t index)
	{
		const glm::vec3& corner = corners[index];
		glVertex3f(corner.x, corner.y, corner.z);
	}
}

namespace ve::rendering
{
	void PrimitiveRenderer3D::DrawSolidCube(const CubeRequest& request) const
	{
		static constexpr std::array<std::size_t, 24> face_indices{
			0, 1, 2, 3, 4, 7, 6, 5, 0, 4, 5, 1,
			1, 5, 6, 2, 2, 6, 7, 3, 3, 7, 4, 0
		};
		const std::array<glm::vec3, 8> corners = BuildCubeCorners(request);
		glDisable(GL_TEXTURE_2D);
		ApplyColor(request.color);
		glBegin(GL_QUADS);
		for (const std::size_t index : face_indices) EmitCorner(corners, index);
		glEnd();
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void PrimitiveRenderer3D::DrawWireCube(const CubeRequest& request, float line_width) const
	{
		static constexpr std::array<std::size_t, 24> edge_indices{
			0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
			6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7
		};
		const std::array<glm::vec3, 8> corners = BuildCubeCorners(request);
		glDisable(GL_TEXTURE_2D);
		glLineWidth(line_width);
		ApplyColor(request.color);
		glBegin(GL_LINES);
		for (const std::size_t index : edge_indices) EmitCorner(corners, index);
		glEnd();
		glLineWidth(1.0f);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void DrawSolidCube(const CubeRequest& request)
	{
		PrimitiveRenderer3D{}.DrawSolidCube(request);
	}

	void DrawWireCube(const CubeRequest& request, float line_width)
	{
		PrimitiveRenderer3D{}.DrawWireCube(request, line_width);
	}
}
