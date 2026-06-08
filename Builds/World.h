#pragma once

#include "Chunk.h"
#include "LevelSpawn.h"

#include <glm/glm.hpp>
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
		 * @param blockRegistry Registry used by chunk meshes.
		 */
		void Draw(const ve::blocks::BlockRegistry& blockRegistry);

		/**
		 * Reads a block from world coordinates.
		 *
		 * @param globalX World X block coordinate.
		 * @param globalY World Y block coordinate.
		 * @param globalZ World Z block coordinate.
		 * @return Block id at the coordinate, or Air when outside loaded chunks.
		 */
		ve::blocks::BlockId GetBlock(int globalX, int globalY, int globalZ) const;

		/**
		 * Reads a block from world coordinates.
		 *
		 * @param position World block coordinate.
		 * @return Block id at the coordinate, or Air when outside loaded chunks.
		 */
		ve::blocks::BlockId GetBlock(const glm::ivec3& position) const;

		/**
		 * Writes a block in world coordinates and marks affected chunks dirty.
		 *
		 * @param globalX World X block coordinate.
		 * @param globalY World Y block coordinate.
		 * @param globalZ World Z block coordinate.
		 * @param blockId New block id.
		 * @return true when the block changed.
		 */
		bool SetBlock(int globalX, int globalY, int globalZ, ve::blocks::BlockId blockId);

		/**
		 * Writes a block in world coordinates and marks affected chunks dirty.
		 *
		 * @param position World block coordinate.
		 * @param blockId New block id.
		 * @return true when the block changed.
		 */
		bool SetBlock(const glm::ivec3& position, ve::blocks::BlockId blockId);

	private:
		/**
		 * Returns the chunk that contains a chunk-grid coordinate.
		 *
		 * @param chunkX Chunk-grid X coordinate.
		 * @param chunkZ Chunk-grid Z coordinate.
		 * @return Mutable chunk pointer, or nullptr when not loaded.
		 */
		Chunk* FindChunk(int chunkX, int chunkZ);

		/**
		 * Returns the chunk that contains a chunk-grid coordinate.
		 *
		 * @param chunkX Chunk-grid X coordinate.
		 * @param chunkZ Chunk-grid Z coordinate.
		 * @return Const chunk pointer, or nullptr when not loaded.
		 */
		const Chunk* FindChunk(int chunkX, int chunkZ) const;

		/**
		 * Marks neighbor chunks dirty when a changed local block touches a border.
		 *
		 * @param chunkX Changed chunk-grid X coordinate.
		 * @param chunkZ Changed chunk-grid Z coordinate.
		 * @param localX Local X block coordinate.
		 * @param localZ Local Z block coordinate.
		 */
		void MarkBorderNeighborsDirty(int chunkX, int chunkZ, int localX, int localZ);

		LevelSpawn _levelSpawn;
		ChunkList _chunks;
		int _worldSize;
	};
}
