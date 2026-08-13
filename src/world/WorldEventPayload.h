#pragma once

#include "Block.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <variant>

namespace ve::world
{
	struct BlockChangedEvent
	{
		glm::ivec3 worldBlockPosition;
		ve::blocks::BlockId previousBlockId;
		ve::blocks::BlockId newBlockId;
	};

	struct ChunkGeneratedEvent
	{
		int chunkCoordinateX;
		int chunkCoordinateZ;
	};

	struct ChunkLoadedEvent
	{
		int chunkCoordinateX = 0;
		int chunkCoordinateZ = 0;
	};

	struct ChunkUnloadedEvent
	{
		int chunkCoordinateX = 0;
		int chunkCoordinateZ = 0;
	};

	struct EntityChangedEvent
	{
		std::uint64_t entityId = 0;
		std::string reason;
	};

	struct RegionSavedEvent
	{
		int minimumChunkCoordinateX = 0;
		int minimumChunkCoordinateZ = 0;
		int maximumChunkCoordinateX = 0;
		int maximumChunkCoordinateZ = 0;
		std::string destination;
	};

	using WorldEventPayload = std::variant<
		BlockChangedEvent,
		ChunkGeneratedEvent,
		ChunkLoadedEvent,
		ChunkUnloadedEvent,
		EntityChangedEvent,
		RegionSavedEvent>;
}
