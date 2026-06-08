#include "World.h"

#include <algorithm>
#include <cmath>

namespace ve::world
{
	namespace
	{
		/**
		 * Estimates arena size for chunk storage.
		 *
		 * @param chunkCount Number of chunks expected to be spawned.
		 * @return Byte count reserved for level chunk containers.
		 */
		std::size_t EstimateWorldArenaBytes(std::size_t chunkCount)
		{
			return chunkCount * sizeof(Chunk);
		}

		/**
		 * Divides and rounds down for block-to-chunk coordinate conversion.
		 *
		 * @param value Coordinate being divided.
		 * @param divisor Positive divisor.
		 * @return Floor-divided value.
		 */
		int FloorDiv(int value, int divisor)
		{
			int result = value / divisor;
			const int remainder = value % divisor;
			if (remainder != 0 && ((remainder < 0) != (divisor < 0)))
			{
				result--;
			}
			return result;
		}

		/**
		 * Computes a positive modulo for local block coordinates.
		 *
		 * @param value Coordinate being reduced.
		 * @param divisor Positive divisor.
		 * @return Positive modulo result.
		 */
		int PositiveMod(int value, int divisor)
		{
			const int result = value % divisor;
			return result < 0 ? result + divisor : result;
		}
	}

	World::World(std::size_t chunkCount)
		: _levelSpawn(EstimateWorldArenaBytes(chunkCount)),
		  _chunks(ChunkAllocator(&_levelSpawn.MemoryResource())),
		  _worldSize(0)
	{
		_chunks.reserve(chunkCount);
	}

	void World::SpawnFlatGrid(int worldSize)
	{
		_worldSize = worldSize;
		for (int x = 0; x < worldSize; x++)
		{
			for (int z = 0; z < worldSize; z++)
			{
				_chunks.emplace_back(x, z);
			}
		}
	}

	void World::Draw(const ve::blocks::BlockRegistry& blockRegistry, const glm::vec3& cameraPosition, int renderDistanceChunks)
	{
		const int cameraChunkX = FloorDiv(static_cast<int>(std::floor(cameraPosition.x)), Chunk::CHUNK_WIDTH);
		const int cameraChunkZ = FloorDiv(static_cast<int>(std::floor(cameraPosition.z)), Chunk::CHUNK_DEPTH);
		const int minChunkX = std::max(0, cameraChunkX - renderDistanceChunks);
		const int maxChunkX = std::min(_worldSize - 1, cameraChunkX + renderDistanceChunks);
		const int minChunkZ = std::max(0, cameraChunkZ - renderDistanceChunks);
		const int maxChunkZ = std::min(_worldSize - 1, cameraChunkZ + renderDistanceChunks);

		for (int chunkX = minChunkX; chunkX <= maxChunkX; chunkX++)
		{
			for (int chunkZ = minChunkZ; chunkZ <= maxChunkZ; chunkZ++)
			{
				Chunk* chunk = FindChunk(chunkX, chunkZ);
				if (!chunk)
				{
					continue;
				}

				chunk->Draw(
					blockRegistry,
					FindChunk(chunkX - 1, chunkZ),
					FindChunk(chunkX + 1, chunkZ),
					FindChunk(chunkX, chunkZ - 1),
					FindChunk(chunkX, chunkZ + 1));
			}
		}
	}

	ve::blocks::BlockId World::GetBlock(int globalX, int globalY, int globalZ) const
	{
		const int chunkX = FloorDiv(globalX, Chunk::CHUNK_WIDTH);
		const int chunkZ = FloorDiv(globalZ, Chunk::CHUNK_DEPTH);
		const Chunk* chunk = FindChunk(chunkX, chunkZ);
		if (!chunk)
		{
			return ve::blocks::BlockId::Air;
		}

		return chunk->GetBlock(
			PositiveMod(globalX, Chunk::CHUNK_WIDTH),
			globalY,
			PositiveMod(globalZ, Chunk::CHUNK_DEPTH));
	}

	ve::blocks::BlockId World::GetBlock(const glm::ivec3& position) const
	{
		return GetBlock(position.x, position.y, position.z);
	}

	bool World::SetBlock(int globalX, int globalY, int globalZ, ve::blocks::BlockId blockId)
	{
		const int chunkX = FloorDiv(globalX, Chunk::CHUNK_WIDTH);
		const int chunkZ = FloorDiv(globalZ, Chunk::CHUNK_DEPTH);
		Chunk* chunk = FindChunk(chunkX, chunkZ);
		if (!chunk)
		{
			return false;
		}

		const int localX = PositiveMod(globalX, Chunk::CHUNK_WIDTH);
		const int localZ = PositiveMod(globalZ, Chunk::CHUNK_DEPTH);
		if (!chunk->SetBlock(localX, globalY, localZ, blockId))
		{
			return false;
		}

		MarkBorderNeighborsDirty(chunkX, chunkZ, localX, localZ);
		return true;
	}

	bool World::SetBlock(const glm::ivec3& position, ve::blocks::BlockId blockId)
	{
		return SetBlock(position.x, position.y, position.z, blockId);
	}

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
			if (Chunk* neighbor = FindChunk(chunkX - 1, chunkZ))
			{
				neighbor->MarkDirty();
			}
		}
		if (localX == Chunk::CHUNK_WIDTH - 1)
		{
			if (Chunk* neighbor = FindChunk(chunkX + 1, chunkZ))
			{
				neighbor->MarkDirty();
			}
		}
		if (localZ == 0)
		{
			if (Chunk* neighbor = FindChunk(chunkX, chunkZ - 1))
			{
				neighbor->MarkDirty();
			}
		}
		if (localZ == Chunk::CHUNK_DEPTH - 1)
		{
			if (Chunk* neighbor = FindChunk(chunkX, chunkZ + 1))
			{
				neighbor->MarkDirty();
			}
		}
	}
}
