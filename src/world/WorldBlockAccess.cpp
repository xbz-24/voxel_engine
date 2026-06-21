#include "World.h"

#include "WorldCoordinates.h"

namespace ve::world
{
	ve::blocks::BlockId World::GetBlock(int globalX, int globalY, int globalZ) const
	{
		const int chunkX = coordinates::FloorDiv(globalX, Chunk::CHUNK_WIDTH);
		const int chunkZ = coordinates::FloorDiv(globalZ, Chunk::CHUNK_DEPTH);
		const Chunk* chunk = FindChunk(chunkX, chunkZ);
		if (!chunk)
		{
			return ve::blocks::BlockId::Air;
		}

		return chunk->GetBlock(
			coordinates::PositiveMod(globalX, Chunk::CHUNK_WIDTH),
			globalY,
			coordinates::PositiveMod(globalZ, Chunk::CHUNK_DEPTH));
	}

	ve::blocks::BlockId World::GetBlock(const glm::ivec3& position) const
	{
		return GetBlock(position.x, position.y, position.z);
	}

	bool World::SetBlock(int globalX, int globalY, int globalZ, ve::blocks::BlockId blockId)
	{
		const int chunkX = coordinates::FloorDiv(globalX, Chunk::CHUNK_WIDTH);
		const int chunkZ = coordinates::FloorDiv(globalZ, Chunk::CHUNK_DEPTH);
		Chunk* chunk = FindChunk(chunkX, chunkZ);
		if (!chunk)
		{
			return false;
		}

		const int localX = coordinates::PositiveMod(globalX, Chunk::CHUNK_WIDTH);
		const int localZ = coordinates::PositiveMod(globalZ, Chunk::CHUNK_DEPTH);
		const ve::blocks::BlockId previousBlockId = chunk->GetBlock(localX, globalY, localZ);
		if (previousBlockId == blockId)
		{
			return true;
		}

		if (!chunk->SetBlock(localX, globalY, localZ, blockId))
		{
			return false;
		}

		++_revision;
		MarkBorderNeighborsDirty(chunkX, chunkZ, localX, localZ);
		RecordBlockChanged(glm::ivec3(globalX, globalY, globalZ), previousBlockId, blockId);
		return true;
	}

	bool World::SetBlock(const glm::ivec3& position, ve::blocks::BlockId blockId)
	{
		return SetBlock(position.x, position.y, position.z, blockId);
	}
}
