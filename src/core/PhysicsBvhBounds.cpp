#include "PhysicsBvhBounds.h"

namespace ve::physics::detail
{
	Aabb MergeBounds(const Aabb& left, const Aabb& right) noexcept
	{
		return { glm::min(left.minimum, right.minimum), glm::max(left.maximum, right.maximum) };
	}

	int WidestAxis(const Aabb& bounds) noexcept
	{
		const glm::vec3 size = bounds.Size();
		if (size.x >= size.y && size.x >= size.z) return 0;
		return size.y >= size.z ? 1 : 2;
	}

	float AxisValue(const glm::vec3& value, int axis) noexcept
	{
		if (axis == 0) return value.x;
		return axis == 1 ? value.y : value.z;
	}
}
