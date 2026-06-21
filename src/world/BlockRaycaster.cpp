#include "BlockRaycaster.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ve::gameplay
{
	namespace
	{
		/**
		 * Converts a world coordinate to its block cell.
		 */
		glm::ivec3 FloorToBlock(const glm::vec3& value)
		{
			return glm::ivec3(
				static_cast<int>(std::floor(value.x)),
				static_cast<int>(std::floor(value.y)),
				static_cast<int>(std::floor(value.z)));
		}

		/**
		 * Returns DDA step direction for one ray component.
		 */
		int StepFor(float direction)
		{
			if (direction > 0.0f) return 1;
			if (direction < 0.0f) return -1;
			return 0;
		}

		/**
		 * Computes distance to the next grid boundary for one axis.
		 */
		float InitialAxisDistance(float origin, float direction, int block, int step)
		{
			if (step == 0) return std::numeric_limits<float>::infinity();
			const float boundary = static_cast<float>(block + (step > 0 ? 1 : 0));
			return (boundary - origin) / direction;
		}

		/**
		 * Computes repeated DDA distance increment for one axis.
		 */
		float AxisDelta(float direction, int step)
		{
			return step == 0 ? std::numeric_limits<float>::infinity() : std::abs(1.0f / direction);
		}
	}

	std::optional<BlockRaycastHit> RaycastBlocks(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, const glm::vec3& origin, const glm::vec3& direction, float maxDistance)
	{
		if (glm::length(direction) <= 0.0f)
		{
			return std::nullopt;
		}

		const glm::vec3 rayDirection = glm::normalize(direction);
		glm::ivec3 currentBlock = FloorToBlock(origin);
		glm::ivec3 previousBlock = currentBlock;
		const glm::ivec3 step(StepFor(rayDirection.x), StepFor(rayDirection.y), StepFor(rayDirection.z));
		glm::vec3 nextDistance(
			InitialAxisDistance(origin.x, rayDirection.x, currentBlock.x, step.x),
			InitialAxisDistance(origin.y, rayDirection.y, currentBlock.y, step.y),
			InitialAxisDistance(origin.z, rayDirection.z, currentBlock.z, step.z));
		const glm::vec3 deltaDistance(AxisDelta(rayDirection.x, step.x), AxisDelta(rayDirection.y, step.y), AxisDelta(rayDirection.z, step.z));

		for (float distance = 0.0f; distance <= maxDistance;)
		{
			if (blockRegistry.IsSolid(world.GetBlock(currentBlock)))
			{
				return BlockRaycastHit{ currentBlock, previousBlock };
			}

			previousBlock = currentBlock;
			if (nextDistance.x <= nextDistance.y && nextDistance.x <= nextDistance.z)
			{
				currentBlock.x += step.x;
				distance = nextDistance.x;
				nextDistance.x += deltaDistance.x;
			}
			else if (nextDistance.y <= nextDistance.z)
			{
				currentBlock.y += step.y;
				distance = nextDistance.y;
				nextDistance.y += deltaDistance.y;
			}
			else
			{
				currentBlock.z += step.z;
				distance = nextDistance.z;
				nextDistance.z += deltaDistance.z;
			}
		}
		return std::nullopt;
	}
}
