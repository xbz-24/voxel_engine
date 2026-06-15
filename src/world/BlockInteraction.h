#pragma once

#include "Block.h"

#include <glm/glm.hpp>

namespace ve::world
{
	class World;
}

namespace ve::gameplay
{
	/**
	 * Type of block mutation requested by gameplay.
	 */
	enum class BlockInteractionType
	{
		Break,
		Place
	};

	/**
	 * Describes one requested block interaction.
	 */
	struct BlockInteraction
	{
		BlockInteractionType type;
		glm::ivec3 position;
		ve::blocks::BlockId blockId;
	};

	/**
	 * Breaks the block at a world position.
	 *
	 * @param world World mutated by the interaction.
	 * @param position Block world position to clear.
	 * @return True when the world accepted the mutation.
	 */
	bool BreakBlock(ve::world::World& world, const glm::ivec3& position);

	/**
	 * Places a block at a world position.
	 *
	 * @param world World mutated by the interaction.
	 * @param position Block world position to write.
	 * @param blockId Block id to place.
	 * @return True when the world accepted the mutation.
	 */
	bool PlaceBlock(ve::world::World& world, const glm::ivec3& position, ve::blocks::BlockId blockId);

	/**
	 * Applies a generic block interaction.
	 *
	 * @param world World mutated by the interaction.
	 * @param interaction Interaction data produced by gameplay systems.
	 * @return True when the world accepted the mutation.
	 */
	bool ApplyBlockInteraction(ve::world::World& world, const BlockInteraction& interaction);
}
