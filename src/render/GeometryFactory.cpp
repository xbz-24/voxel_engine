#include "GeometryFactory.h"

#include "MeshBuilder.h"

#include <utility>

namespace ve::rendering
{
	/** Builds a single-triangle mesh. */
	IndexedMesh GeometryFactory::Triangle(glm::vec3 first, glm::vec3 second, glm::vec3 third, ColorRgba color)
	{
		MeshBuilder builder;
		builder.Reserve(3U, 3U).AddTriangle(first, second, third, color);
		return std::move(builder).Build();
	}

	/** Builds a centered quad mesh on the XY plane. */
	IndexedMesh GeometryFactory::Quad(glm::vec2 rect, ColorRgba color)
	{
		const glm::vec3 half_size{ rect.x * 0.5f, rect.y * 0.5f, 0.0f };
		MeshBuilder builder;
		builder.Reserve(4U, 6U).AddQuad(
			{ -half_size.x, -half_size.y, 0.0f }, { half_size.x, -half_size.y, 0.0f },
			{ half_size.x, half_size.y, 0.0f }, { -half_size.x, half_size.y, 0.0f }, color);
		return std::move(builder).Build();
	}

	/** Builds an indexed cube mesh. */
	IndexedMesh GeometryFactory::Cube(glm::vec3 center, glm::vec3 size, ColorRgba color)
	{
		MeshBuilder builder;
		builder.Reserve(24U, 36U).AddCube(center, size, color);
		return std::move(builder).Build();
	}
}
