#pragma once

#include "Block.h"
#include "VulkanGpuChunkGeometry.h"

#include <cstddef>

namespace ve::world
{
	class World;
}

namespace ve::rendering::detail
{
	[[nodiscard]] float BlockLightBoost(ve::blocks::BlockId block) noexcept;
	[[nodiscard]] float CornerOcclusion(
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		const BlockFaceGeometry& face,
		const glm::ivec3& block_coordinate,
		std::size_t corner) noexcept;
}
