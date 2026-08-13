#include "BlockGridRaycaster.h"

#include "WorldGridMath.h"

#include <cmath>
#include <limits>

namespace ve::gameplay::detail
{
	glm::ivec3 FloorToBlock(const glm::vec3& value)
	{
		return ve::world::grid::BlockPositionFromWorld(value);
	}

	int StepFor(float direction) noexcept
	{
		if (direction > 0.0f) return 1;
		if (direction < 0.0f) return -1;
		return 0;
	}

	float InitialAxisDistance(float origin, float direction, int block, int step) noexcept
	{
		if (step == 0) return std::numeric_limits<float>::infinity();
		const float boundary = ve::world::grid::GridBoundaryForStep(block, step);
		return (boundary - origin) / direction;
	}

	float AxisDelta(float direction, int step) noexcept
	{
		return step == 0 ? std::numeric_limits<float>::infinity() : std::abs(1.0f / direction);
	}
}
