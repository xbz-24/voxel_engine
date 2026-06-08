#pragma once

#include "Chunk.h"
#include "LevelSpawn.h"

#include <memory_resource>
#include <vector>

namespace ve::world
{
	using ChunkAllocator = std::pmr::polymorphic_allocator<Chunk>;
	using ChunkList = std::vector<Chunk, ChunkAllocator>;

	/**
	 * Owns world chunks and the level-lifetime memory arena used by them.
	 */
	class World
	{
	public:
		/**
		 * Creates an empty world with enough arena memory for the requested chunks.
		 *
		 * @param chunkCount Number of chunks expected to be spawned.
		 */
		explicit World(std::size_t chunkCount);

		World(const World&) = delete;
		World& operator=(const World&) = delete;
		World(World&&) = delete;
		World& operator=(World&&) = delete;

		/**
		 * Spawns a square grid of generated chunks.
		 *
		 * @param worldSize Number of chunks on the X and Z axes.
		 */
		void SpawnFlatGrid(int worldSize);

		/**
		 * Draws all chunks in the world.
		 *
		 * @param cubeManager Cube texture provider used by chunk meshes.
		 */
		void Draw(Cube& cubeManager);

	private:
		LevelSpawn _levelSpawn;
		ChunkList _chunks;
	};
}

