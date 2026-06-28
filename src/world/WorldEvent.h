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
		// TODO: Add ChunkLoaded, ChunkUnloaded, EntityChanged, and RegionSaved events before tooling relies on polling.
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
		// TODO: Replace parallel payload fields with std::variant once event volume and type count grow.
		WorldEventType eventType;
		BlockChangedEvent blockChanged;
		ChunkGeneratedEvent chunkGenerated;
	};
}
