#include "Frustum.h"

#include <glm/ext.hpp>

namespace
{
	/// Returns a row from a column-major GLM matrix.
	glm::vec4 Row(const glm::mat4& matrix, int row)
	{
		return glm::vec4(matrix[0][row], matrix[1][row], matrix[2][row], matrix[3][row]);
	}

	/// Normalizes one plane equation.
	glm::vec4 NormalizePlane(const glm::vec4& plane)
	{
		const float length = glm::length(glm::vec3(plane));
		if (length <= 0.0001f) return plane;
		return plane / length;
	}

	/// Selects the AABB vertex most aligned with a plane normal.
	glm::vec3 PositiveVertex(const glm::vec4& plane, const glm::vec3& minimum_corner, const glm::vec3& maximum_corner)
	{
		return glm::vec3(
			plane.x >= 0.0f ? maximum_corner.x : minimum_corner.x,
			plane.y >= 0.0f ? maximum_corner.y : minimum_corner.y,
			plane.z >= 0.0f ? maximum_corner.z : minimum_corner.z);
	}
}

namespace ve::rendering
{
	Frustum Frustum::FromViewProjection(const glm::mat4& view_projection)
	{
		const glm::vec4 row0 = Row(view_projection, 0);
		const glm::vec4 row1 = Row(view_projection, 1);
		const glm::vec4 row2 = Row(view_projection, 2);
		const glm::vec4 row3 = Row(view_projection, 3);
		Frustum frustum;
		frustum.planes_ = {{
			NormalizePlane(row3 + row0),
			NormalizePlane(row3 - row0),
			NormalizePlane(row3 + row1),
			NormalizePlane(row3 - row1),
			NormalizePlane(row3 + row2),
			NormalizePlane(row3 - row2)
		}};
		return frustum;
	}

	bool Frustum::IntersectsAabb(const glm::vec3& minimum_corner, const glm::vec3& maximum_corner) const
	{
		for (const glm::vec4& plane : planes_)
		{
			const glm::vec3 vertex = PositiveVertex(plane, minimum_corner, maximum_corner);
			if (glm::dot(glm::vec3(plane), vertex) + plane.w < 0.0f) return false;
		}
		return true;
	}
}
