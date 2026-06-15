#pragma once

#include "Chunk.h"
#include "AsyncWorldGenerator.h"
#include "ChunkMeshPipeline.h"
#include "ChunkMeshRequest.h"
#include "ChunkMeshTypes.h"
#include "LevelSpawn.h"
#include "WorldConfiguration.h"
#include "WorldEvent.h"
#include "WorldRenderRequest.h"

#include <glm/glm.hpp>
#include <memory_resource>
#include <optional>
#include <vector>

namespace ve::world
{
	using ChunkAllocator = std::pmr::polymorphic_allocator<Chunk>;
	using ChunkList = std::vector<Chunk, ChunkAllocator>;

	class World
	{
	public:
		/** @param createInfo Chunk capacity and arena sizing data. */
		explicit World(const WorldCreateInfo& createInfo);

		/** @param chunkCount Number of chunks to reserve in the level arena. */
		explicit World(std::size_t chunkCount);

		World(const World&) = delete;
		World& operator=(const World&) = delete;
		World(World&&) = delete;
		World& operator=(World&&) = delete;

		/** @param worldSize Number of chunks along each world side. */
		void SpawnFlatGrid(int worldSize);

		/** @param settings Spawn settings containing the world size in chunks. */
		void SpawnFlatGrid(const FlatWorldSpawnSettings& settings);

		/** @param settings Spawn settings containing the world size in chunks. */
		void SpawnEmptyGrid(const FlatWorldSpawnSettings& settings);

		/** @param result Generated terrain result produced by a worker. @return True when applied. */
		bool ApplyGeneratedChunk(const generation::ChunkGenerationResult& result);

		/** @param request Camera and render data used to submit visible chunks. */
		void Draw(const WorldRenderRequest& request);

		/// Reads a block from world coordinates.
		ve::blocks::BlockId GetBlock(int globalX, int globalY, int globalZ) const;

		/// Reads a block from world coordinates.
		ve::blocks::BlockId GetBlock(const glm::ivec3& position) const;

		/// Writes a block in world coordinates and marks affected chunks dirty.
		bool SetBlock(int globalX, int globalY, int globalZ, ve::blocks::BlockId blockId);

		/// Writes a block in world coordinates and marks affected chunks dirty.
		bool SetBlock(const glm::ivec3& position, ve::blocks::BlockId blockId);

		/** @return Snapshot of world size and chunk storage counts. */
		WorldMetrics Metrics() const noexcept;

		/** @return Events emitted since the previous drain. */
		std::vector<WorldEvent> DrainEvents();

		/** @return Number of events waiting for consumers. */
		std::size_t PendingEventCount() const noexcept;

		/** @param chunkX Chunk-grid X coordinate. @param chunkZ Chunk-grid Z coordinate. @return Snapshot request, or empty for missing chunks. */
		std::optional<ve::world::mesh::ChunkMeshBuildRequest> CaptureChunkMeshBuildRequest(int chunkX, int chunkZ) const;

		/** @param output Completed CPU mesh output. @return True when it matched a loaded chunk. */
		bool TryUploadChunkMeshOutput(ve::world::mesh::ChunkMeshBuildOutput output);

		/** @param meshPipeline Async mesh pipeline receiving visible chunk work. */
		void UploadReadyChunkMeshes(ve::world::mesh::ChunkMeshPipeline& meshPipeline);

		/** @param blockRegistry Block metadata. @param meshPipeline Async mesh pipeline. @param cameraPosition Camera world position. @param renderDistanceChunks Chunk radius. */
		void ScheduleVisibleChunkMeshes(const ve::blocks::BlockRegistry& blockRegistry, ve::world::mesh::ChunkMeshPipeline& meshPipeline, const glm::vec3& cameraPosition, int renderDistanceChunks);

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

		/// Marks a generated chunk and its direct neighbors dirty.
		void MarkGeneratedChunkNeighborhoodDirty(int chunkX, int chunkZ);

		LevelSpawn _levelSpawn;
		ChunkList _chunks;
		std::vector<WorldEvent> _pendingEvents;
		int _worldSize;
	};
}
