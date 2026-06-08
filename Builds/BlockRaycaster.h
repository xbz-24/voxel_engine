#pragma once

#include "BlockRegistry.h"
#include "World.h"

#include <glm/glm.hpp>
#include <optional>

namespace ve::gameplay
{
	/**
	 * Result of a voxel grid raycast.
	 */
	struct BlockRaycastHit
	{
		glm::ivec3 targetBlock;
		glm::ivec3 placementBlock;
	};

	/**
	 * Traverses the block grid and returns the first solid block hit.
	 *
	 * @param world World queried for block ids.
	 * @param blockRegistry Registry used to test solidity.
	 * @param origin Ray origin in world space.
	 * @param direction Ray direction in world space.
	 * @param maxDistance Maximum ray distance in blocks.
	 * @return Hit data when a solid block is found.
	 */
	std::optional<BlockRaycastHit> RaycastBlocks(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, const glm::vec3& origin, const glm::vec3& direction, float maxDistance);
}
