#include "BlockGridRaycaster.h"

#include <glm/geometric.hpp>

namespace ve::gameplay
{
	std::optional<BlockRaycastHit> RaycastBlocks(const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry, const glm::vec3& origin,
		const glm::vec3& direction, float max_distance)
	{
		if (glm::length(direction) <= 0.0f)
			return std::nullopt;
		return detail::BlockGridRaycaster{
			world, block_registry, origin, direction, max_distance }.Trace();
	}
}
