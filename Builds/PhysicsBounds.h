#pragma once

#include <glm/glm.hpp>

namespace ve::physics
{
	struct Aabb
	{
		glm::vec3 minimum{ 0.0f };
		glm::vec3 maximum{ 0.0f };

		/** @param point Point to test. @return True when the point is inside this box. */
		[[nodiscard]] bool Contains(const glm::vec3& point) const noexcept;

		/** @param other Box to test. @return True when both boxes overlap. */
		[[nodiscard]] bool Intersects(const Aabb& other) const noexcept;

		/** @return Center point of the box. */
		[[nodiscard]] glm::vec3 Center() const noexcept;

		/** @return Box extents along each axis. */
		[[nodiscard]] glm::vec3 Size() const noexcept;
	};

	struct PhysicsProxy
	{
		unsigned int id = 0;
		Aabb bounds{};
	};
}
