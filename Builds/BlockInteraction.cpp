#include "BlockInteraction.h"

#include "World.h"

namespace ve::gameplay
{
	/// Breaks the block at a world position.
	bool BreakBlock(ve::world::World& world, const glm::ivec3& position)
	{
		return world.SetBlock(position, ve::blocks::BlockId::Air);
	}

	/// Places a block at a world position.
	bool PlaceBlock(ve::world::World& world, const glm::ivec3& position, ve::blocks::BlockId blockId)
	{
		return world.SetBlock(position, blockId);
	}

	/// Applies a generic block interaction.
	bool ApplyBlockInteraction(ve::world::World& world, const BlockInteraction& interaction)
	{
		switch (interaction.type)
		{
		case BlockInteractionType::Break:
			return BreakBlock(world, interaction.position);
		case BlockInteractionType::Place:
			return PlaceBlock(world, interaction.position, interaction.blockId);
		}
		return false;
	}
}
