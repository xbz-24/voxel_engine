#include "World.h"

#include <array>
#include <utility>

namespace ve::world
{
	namespace
	{
		constexpr std::array kGeneratedChunkMeshDependencyOffsets{
			std::pair{ 0, 0 },
			std::pair{ -1, 0 },
			std::pair{ 1, 0 },
			std::pair{ 0, -1 },
			std::pair{ 0, 1 }
		};
	}

	/// Applies generated terrain data to a loaded chunk.
	bool World::ApplyGeneratedChunk(const generation::ChunkGenerationResult& result)
	{
		Chunk* chunk = FindChunk(result.chunkCoordinateX, result.chunkCoordinateZ);
		if (!chunk || !chunk->ReplaceBlocks(result.blocks)) return false;
		++_revision;
		MarkGeneratedChunkNeighborhoodDirty(result.chunkCoordinateX, result.chunkCoordinateZ);
		RecordChunkGenerated(result.chunkCoordinateX, result.chunkCoordinateZ);
		return true;
	}

	/// Marks a generated chunk and its direct neighbors dirty.
	void World::MarkGeneratedChunkNeighborhoodDirty(int chunkX, int chunkZ)
	{
		for (const auto [offsetX, offsetZ] : kGeneratedChunkMeshDependencyOffsets)
		{
			if (Chunk* chunk = FindChunk(chunkX + offsetX, chunkZ + offsetZ)) MarkChunkDirty(*chunk);
		}
	}
}
