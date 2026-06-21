#pragma once

#include <array>
#include <glm/glm.hpp>

namespace ve::rendering
{
	class Frustum
	{
	public:
		/**
		 * Extracts six frustum planes from a view-projection matrix.
		 *
		 * @param view_projection Combined projection * view matrix.
		 * @return Normalized world-space frustum planes.
		 */
		static Frustum FromViewProjection(const glm::mat4& view_projection);

		/**
		 * Tests whether an axis-aligned box intersects this frustum.
		 *
		 * @param minimum_corner Minimum x/y/z box corner.
		 * @param maximum_corner Maximum x/y/z box corner.
		 * @return True when the box is at least partially visible.
		 */
		bool IntersectsAabb(const glm::vec3& minimum_corner, const glm::vec3& maximum_corner) const;

	private:
		std::array<glm::vec4, 6> planes_;
	};
}
