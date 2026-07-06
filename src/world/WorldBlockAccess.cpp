#include "World.h"

#include "WorldCoordinates.h"

namespace ve::world
{
	ve::blocks::BlockId World::GetBlock(int globalBlockX, int globalBlockY, int globalBlockZ) const
	{
		const int chunkCoordinateX = coordinates::FloorDiv(globalBlockX, Chunk::CHUNK_WIDTH);
		const int chunkCoordinateZ = coordinates::FloorDiv(globalBlockZ, Chunk::CHUNK_DEPTH);
		const Chunk* chunk = FindChunk(chunkCoordinateX, chunkCoordinateZ);
		if (!chunk)
		{
			return ve::blocks::BlockId::Air;
		}

		return chunk->GetBlock(
			coordinates::PositiveMod(globalBlockX, Chunk::CHUNK_WIDTH),
			globalBlockY,
			coordinates::PositiveMod(globalBlockZ, Chunk::CHUNK_DEPTH));
	}

	ve::blocks::BlockId World::GetBlock(const glm::ivec3& position) const
	{
		return GetBlock(position.x, position.y, position.z);
	}

	bool World::SetBlock(int globalBlockX, int globalBlockY, int globalBlockZ, ve::blocks::BlockId blockId)
	{
		if (globalBlockY < 0 || globalBlockY >= Chunk::CHUNK_HEIGHT) return false;
		const int chunkCoordinateX = coordinates::FloorDiv(globalBlockX, Chunk::CHUNK_WIDTH);
		const int chunkCoordinateZ = coordinates::FloorDiv(globalBlockZ, Chunk::CHUNK_DEPTH);
		Chunk* chunk = FindChunk(chunkCoordinateX, chunkCoordinateZ);
		if (!chunk)
		{
			return false;
		}

		const int localBlockX = coordinates::PositiveMod(globalBlockX, Chunk::CHUNK_WIDTH);
		const int localBlockZ = coordinates::PositiveMod(globalBlockZ, Chunk::CHUNK_DEPTH);
		const ve::blocks::BlockId previousBlockId = chunk->GetBlock(localBlockX, globalBlockY, localBlockZ);
		if (previousBlockId == blockId)
		{
			return true;
		}

		if (!chunk->SetBlock(localBlockX, globalBlockY, localBlockZ, blockId))
		{
			return false;
		}

		RecordDirtyChunk(*chunk);
		++_revision;
		MarkBorderNeighborsDirty(chunkCoordinateX, chunkCoordinateZ, localBlockX, localBlockZ);
		RecordBlockChanged(glm::ivec3(globalBlockX, globalBlockY, globalBlockZ), previousBlockId, blockId);
		return true;
	}

	bool World::SetBlock(const glm::ivec3& position, ve::blocks::BlockId blockId)
	{
		return SetBlock(position.x, position.y, position.z, blockId);
	}
}
