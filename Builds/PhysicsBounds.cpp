#include "PhysicsBounds.h"

namespace ve::physics
{
	/// Reports whether a point is inside this axis-aligned box.
	bool Aabb::Contains(const glm::vec3& point) const noexcept
	{
		return point.x >= minimum.x && point.x <= maximum.x &&
			point.y >= minimum.y && point.y <= maximum.y &&
			point.z >= minimum.z && point.z <= maximum.z;
	}

	/// Reports whether two axis-aligned boxes overlap.
	bool Aabb::Intersects(const Aabb& other) const noexcept
	{
		return minimum.x <= other.maximum.x && maximum.x >= other.minimum.x &&
			minimum.y <= other.maximum.y && maximum.y >= other.minimum.y &&
			minimum.z <= other.maximum.z && maximum.z >= other.minimum.z;
	}

	/// Returns the center point of the box.
	glm::vec3 Aabb::Center() const noexcept { return (minimum + maximum) * 0.5f; }

	/// Returns the box size along each axis.
	glm::vec3 Aabb::Size() const noexcept { return maximum - minimum; }
}
