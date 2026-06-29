	std::optional<BlockRaycastHit> RaycastBlocks(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, const glm::vec3& origin, const glm::vec3& direction, float maxDistance)
	{
		if (glm::length(direction) <= 0.0f)
		{
			return std::nullopt;
		}
		return BlockGridRaycaster{ world, blockRegistry, origin, direction, maxDistance }.Trace();
	}
}
