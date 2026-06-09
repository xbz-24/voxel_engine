#pragma once

#include "Chunk.h"
#include "ChunkMeshTypes.h"
#include "LevelSpawn.h"
#include "WorldConfiguration.h"
#include "WorldEvent.h"
#include "WorldRenderRequest.h"

#include <glm/glm.hpp>
#include <memory_resource>
#include <vector>

namespace ve::world
{
	using ChunkAllocator = std::pmr::polymorphic_allocator<Chunk>;
	using ChunkList = std::vector<Chunk, ChunkAllocator>;

	class World
	{
	public:
		/**
		 * Creates an empty world from explicit memory settings.
		 *
		 * @param createInfo Chunk capacity and arena sizing data.
		 */
		explicit World(const WorldCreateInfo& createInfo);

		/**
		 * Creates an empty world with enough arena memory for the requested chunks.
		 *
		 * @param chunkCount Number of chunks to reserve in the level arena.
		 */
		explicit World(std::size_t chunkCount);

		World(const World&) = delete;
		World& operator=(const World&) = delete;
		World(World&&) = delete;
		World& operator=(World&&) = delete;

		/**
		 * Spawns a square grid of generated chunks.
		 *
		 * @param worldSize Number of chunks along each world side.
		 */
		void SpawnFlatGrid(int worldSize);

		/**
		 * Spawns a square grid of generated chunks.
		 *
		 * @param settings Spawn settings containing the world size in chunks.
		 */
		void SpawnFlatGrid(const FlatWorldSpawnSettings& settings);

		/**
		 * Draws chunks described by a render request after visibility culling.
		 *
		 * @param request Camera and render data used to submit visible chunks.
		 */
		void Draw(const WorldRenderRequest& request);

		/// Reads a block from world coordinates.
		ve::blocks::BlockId GetBlock(int globalX, int globalY, int globalZ) const;

		/// Reads a block from world coordinates.
		ve::blocks::BlockId GetBlock(const glm::ivec3& position) const;

		/// Writes a block in world coordinates and marks affected chunks dirty.
		bool SetBlock(int globalX, int globalY, int globalZ, ve::blocks::BlockId blockId);

		/// Writes a block in world coordinates and marks affected chunks dirty.
		bool SetBlock(const glm::ivec3& position, ve::blocks::BlockId blockId);

		/**
		 * Returns loaded chunk and capacity diagnostics.
		 *
		 * @return Snapshot of world size and chunk storage counts.
		 */
		WorldMetrics Metrics() const noexcept;

		/**
		 * Moves pending world events out of the world.
		 *
		 * @return Events emitted since the previous drain.
		 */
		std::vector<WorldEvent> DrainEvents();

		/**
		 * Reports pending events without draining them.
		 *
		 * @return Number of events waiting for consumers.
		 */
		std::size_t PendingEventCount() const noexcept;

	private:
		/// Returns the chunk that contains a chunk-grid coordinate.
		Chunk* FindChunk(int chunkX, int chunkZ);

		/// Returns the chunk that contains a chunk-grid coordinate.
		const Chunk* FindChunk(int chunkX, int chunkZ) const;

		/// Marks neighbor chunks dirty when a changed local block touches a border.
		void MarkBorderNeighborsDirty(int chunkX, int chunkZ, int localX, int localZ);

		/// Draws one chunk when it passes camera-direction culling.
		void DrawVisibleChunk(const WorldRenderRequest& request, int chunkX, int chunkZ);

		/// Collects neighboring chunks used to hide shared border faces.
		ve::world::mesh::NeighborChunks FindNeighborChunks(int chunkX, int chunkZ) const;

		/// Records a generated chunk event.
		void RecordChunkGenerated(int chunkX, int chunkZ);

		/// Records a block changed event.
		void RecordBlockChanged(const glm::ivec3& position, ve::blocks::BlockId previousBlockId, ve::blocks::BlockId newBlockId);

		LevelSpawn _levelSpawn;
		ChunkList _chunks;
		std::vector<WorldEvent> _pendingEvents;
		int _worldSize;
	};
}
