#pragma once

#include "PhysicsBounds.h"

namespace ve::physics::detail
{
	Aabb MergeBounds(const Aabb& left, const Aabb& right) noexcept;
	int WidestAxis(const Aabb& bounds) noexcept;
	float AxisValue(const glm::vec3& value, int axis) noexcept;
}
