#include "GraphicsTypes.h"

#include <algorithm>
#include <limits>

namespace ve::rendering
{
	/** Creates invalid bounds that become valid after the first expansion. */
	GraphicsAabb GraphicsAabb::Empty() noexcept
	{
		constexpr float high = (std::numeric_limits<float>::max)();
		constexpr float low = (std::numeric_limits<float>::lowest)();
		return { glm::vec3{ high }, glm::vec3{ low } };
	}

	/** Grows this bounds to include a point. */
	void GraphicsAabb::Expand(const glm::vec3& point) noexcept
	{
		minimum.x = std::min(minimum.x, point.x);
		minimum.y = std::min(minimum.y, point.y);
		minimum.z = std::min(minimum.z, point.z);
		maximum.x = std::max(maximum.x, point.x);
		maximum.y = std::max(maximum.y, point.y);
		maximum.z = std::max(maximum.z, point.z);
	}

	/** Reports whether a point is inside this bounds. */
	bool GraphicsAabb::Contains(const glm::vec3& point) const noexcept
	{
		return point.x >= minimum.x && point.x <= maximum.x &&
			point.y >= minimum.y && point.y <= maximum.y &&
			point.z >= minimum.z && point.z <= maximum.z;
	}

	/** Reports whether this bounds contains at least one expanded point. */
	bool GraphicsAabb::IsValid() const noexcept
	{
		return minimum.x <= maximum.x && minimum.y <= maximum.y && minimum.z <= maximum.z;
	}

	/** Returns the center point of the bounds. */
	glm::vec3 GraphicsAabb::Center() const noexcept { return (minimum + maximum) * 0.5f; }

	/** Returns the bounds size along each axis. */
	glm::vec3 GraphicsAabb::Size() const noexcept { return maximum - minimum; }
}
