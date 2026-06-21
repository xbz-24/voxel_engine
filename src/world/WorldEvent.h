#pragma once

#include "Block.h"

#include <glm/glm.hpp>

namespace ve::world
{
	/**
	 * Event categories emitted by the world simulation.
	 */
	enum class WorldEventType
	{
		BlockChanged,
		ChunkGenerated
	};

	/**
	 * Describes one block mutation accepted by the world.
	 */
	struct BlockChangedEvent
	{
		glm::ivec3 worldBlockPosition;
		ve::blocks::BlockId previousBlockId;
		ve::blocks::BlockId newBlockId;
	};

	/**
	 * Describes one generated chunk entering the world.
	 */
	struct ChunkGeneratedEvent
	{
		int chunkCoordinateX;
		int chunkCoordinateZ;
	};

	/**
	 * Single event record emitted by world APIs.
	 */
	struct WorldEvent
	{
		WorldEventType eventType;
		BlockChangedEvent blockChanged;
		ChunkGeneratedEvent chunkGenerated;
	};
}
