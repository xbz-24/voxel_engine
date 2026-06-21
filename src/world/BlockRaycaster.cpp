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

		class BlockGridRaycaster
		{
		public:
			BlockGridRaycaster(const ve::world::World& world,
				const ve::blocks::BlockRegistry& block_registry,
				const glm::vec3& origin,
				const glm::vec3& direction,
				float max_distance)
				: world_(world),
				  block_registry_(block_registry),
				  max_distance_(max_distance),
				  ray_direction_(glm::normalize(direction)),
				  current_block_(FloorToBlock(origin)),
				  previous_block_(current_block_),
				  step_(StepFor(ray_direction_.x), StepFor(ray_direction_.y), StepFor(ray_direction_.z)),
				  next_distance_(
					  InitialAxisDistance(origin.x, ray_direction_.x, current_block_.x, step_.x),
					  InitialAxisDistance(origin.y, ray_direction_.y, current_block_.y, step_.y),
					  InitialAxisDistance(origin.z, ray_direction_.z, current_block_.z, step_.z)),
				  delta_distance_(AxisDelta(ray_direction_.x, step_.x), AxisDelta(ray_direction_.y, step_.y), AxisDelta(ray_direction_.z, step_.z))
			{
			}

			[[nodiscard]] std::optional<BlockRaycastHit> Trace()
			{
				for (float distance = 0.0f; distance <= max_distance_;)
				{
					if (block_registry_.IsSolid(world_.GetBlock(current_block_)))
					{
						return BlockRaycastHit{ current_block_, previous_block_ };
					}
					distance = StepToNextBlock();
				}
				return std::nullopt;
			}

		private:
			[[nodiscard]] float StepToNextBlock()
			{
				previous_block_ = current_block_;
				if (next_distance_.x <= next_distance_.y && next_distance_.x <= next_distance_.z)
				{
					current_block_.x += step_.x;
					const float distance = next_distance_.x;
					next_distance_.x += delta_distance_.x;
					return distance;
				}
				if (next_distance_.y <= next_distance_.z)
				{
					current_block_.y += step_.y;
					const float distance = next_distance_.y;
					next_distance_.y += delta_distance_.y;
					return distance;
				}
				current_block_.z += step_.z;
				const float distance = next_distance_.z;
				next_distance_.z += delta_distance_.z;
				return distance;
			}

			const ve::world::World& world_;
			const ve::blocks::BlockRegistry& block_registry_;
			float max_distance_;
			glm::vec3 ray_direction_;
			glm::ivec3 current_block_;
			glm::ivec3 previous_block_;
			glm::ivec3 step_;
			glm::vec3 next_distance_;
			glm::vec3 delta_distance_;
		};
	}

	std::optional<BlockRaycastHit> RaycastBlocks(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, const glm::vec3& origin, const glm::vec3& direction, float maxDistance)
	{
		if (glm::length(direction) <= 0.0f)
		{
			return std::nullopt;
		}
		return BlockGridRaycaster{ world, blockRegistry, origin, direction, maxDistance }.Trace();
	}
}
