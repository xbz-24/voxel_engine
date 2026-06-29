#pragma once

#include "VulkanSoftwareVoxelRasterizerData.h"

#include <glm/glm.hpp>

namespace ve::rendering::raster_ray
{
	struct TraversalState
	{
		glm::ivec3 current_block{};
		glm::ivec3 block_step{};
		glm::vec3 next_axis_distance{};
		glm::vec3 axis_distance_delta{};
		glm::ivec3 surface_normal{ 0, 1, 0 };
		float traveled_distance = 0.0f;
	};

	[[nodiscard]] bool IsSolidBlock(ve::blocks::BlockId block) noexcept;
	[[nodiscard]] int FloorBlockCoordinate(float value) noexcept;
	[[nodiscard]] bool IsInsideWorld(const glm::ivec3& block, const VulkanRasterFrameWorldSnapshot& world) noexcept;
	[[nodiscard]] bool IsLeavingWorld(
		const glm::ivec3& block,
		const glm::ivec3& block_step,
		const VulkanRasterFrameWorldSnapshot& world) noexcept;
	[[nodiscard]] TraversalState StartTraversal(const glm::vec3& origin, const glm::vec3& direction) noexcept;
	void AdvanceToNextBlock(TraversalState& traversal) noexcept;
}
