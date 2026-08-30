#include "VulkanSoftwareVoxelRasterizerRayTraversal.h"

#include <cmath>
#include <limits>

namespace ve::rendering::raster_ray
{
	namespace
	{
		[[nodiscard]] int StepDirection(float direction) noexcept
		{
			if (direction > 0.0f) return 1;
			if (direction < 0.0f) return -1;
			return 0;
		}

		[[nodiscard]] float InitialAxisDistance(float origin, float direction) noexcept
		{
			if (direction > 0.0f) return (std::floor(origin) + 1.0f - origin) / direction;
			if (direction < 0.0f) return (origin - std::floor(origin)) / -direction;
			return std::numeric_limits<float>::infinity();
		}

		[[nodiscard]] float AxisDelta(float direction) noexcept
		{
			return direction == 0.0f ? std::numeric_limits<float>::infinity() : std::abs(1.0f / direction);
		}
	}

	TraversalState StartTraversal(const glm::vec3& origin, const glm::vec3& direction) noexcept
	{
		return {
			.current_block = {
				FloorBlockCoordinate(origin.x),
				FloorBlockCoordinate(origin.y),
				FloorBlockCoordinate(origin.z)
			},
			.block_step = {
				StepDirection(direction.x),
				StepDirection(direction.y),
				StepDirection(direction.z)
			},
			.next_axis_distance = {
				InitialAxisDistance(origin.x, direction.x),
				InitialAxisDistance(origin.y, direction.y),
				InitialAxisDistance(origin.z, direction.z)
			},
			.axis_distance_delta = {
				AxisDelta(direction.x),
				AxisDelta(direction.y),
				AxisDelta(direction.z)
			}
		};
	}
}
