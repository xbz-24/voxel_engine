#include "World.h"

namespace ve::world
{
	Chunk* World::FindChunk(int chunkX, int chunkZ)
	{
		if (chunkX < 0 || chunkZ < 0 || chunkX >= _worldSize || chunkZ >= _worldSize)
		{
			return nullptr;
		}
		return &_chunks[static_cast<std::size_t>(chunkX * _worldSize + chunkZ)];
	}

	const Chunk* World::FindChunk(int chunkX, int chunkZ) const
	{
		if (chunkX < 0 || chunkZ < 0 || chunkX >= _worldSize || chunkZ >= _worldSize)
		{
			return nullptr;
		}
		return &_chunks[static_cast<std::size_t>(chunkX * _worldSize + chunkZ)];
	}

	void World::MarkBorderNeighborsDirty(int chunkX, int chunkZ, int localX, int localZ)
	{
		if (localX == 0)
		{
			if (Chunk* neighbor = FindChunk(chunkX - 1, chunkZ)) neighbor->MarkDirty();
		}
		if (localX == Chunk::CHUNK_WIDTH - 1)
		{
			if (Chunk* neighbor = FindChunk(chunkX + 1, chunkZ)) neighbor->MarkDirty();
		}
		if (localZ == 0)
		{
			if (Chunk* neighbor = FindChunk(chunkX, chunkZ - 1)) neighbor->MarkDirty();
		}
		if (localZ == Chunk::CHUNK_DEPTH - 1)
		{
			if (Chunk* neighbor = FindChunk(chunkX, chunkZ + 1)) neighbor->MarkDirty();
		}
	}
}
