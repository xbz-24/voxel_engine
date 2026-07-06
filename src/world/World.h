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
#include <cstdint>
#include <memory>
#include <memory_resource>
#include <optional>
#include <span>
#include <vector>

namespace ve::rendering
{
	class RenderBackend;
	class RenderMesh;
}

namespace ve::world
{
	using ChunkAllocator = std::pmr::polymorphic_allocator<Chunk>;
	using ChunkList = std::vector<Chunk, ChunkAllocator>;

	struct DirtyChunkMetadata
	{
		int chunk_x = 0;
		int chunk_z = 0;
		std::uint64_t mesh_revision = 0;
		bool has_authored_edits = false;
	};

	// TODO: Split storage, generation, meshing, and event publication so World can be used headlessly by tools/tests.
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

		/** @param renderBackend Backend used to create mesh resources for chunks spawned after this call. */
		void SetRenderBackend(const ve::rendering::RenderBackend* renderBackend) noexcept;

		/** @param worldSize Number of chunks along each world side. */
		void SpawnFlatGrid(int worldSize);
		// TODO: Add streaming world bounds so large worlds are not limited to one fixed square grid.

		/** @param settings Spawn settings containing the world size in chunks. */
		void SpawnFlatGrid(const FlatWorldSpawnSettings& settings);

		/** @param settings Spawn settings containing the world size in chunks. */
		void SpawnEmptyGrid(const FlatWorldSpawnSettings& settings);

		/** @param result Generated terrain result produced by a worker. @return True when applied. */
		bool ApplyGeneratedChunk(const generation::ChunkGenerationResult& result);

		/** @param request Camera and render data used to submit visible chunks. */
		void Draw(const WorldRenderRequest& request);

		/** @param request Camera and render data used to collect visible chunk meshes. @return Visible render items. */
		ChunkRenderItemList ExtractVisibleChunks(const WorldRenderRequest& request) const;

		/// Reads a block from world coordinates.
		ve::blocks::BlockId GetBlock(int globalBlockX, int globalBlockY, int globalBlockZ) const;

		/// Reads a block from world coordinates.
		ve::blocks::BlockId GetBlock(const glm::ivec3& position) const;

		/// Writes a block in world coordinates and marks affected chunks dirty.
		bool SetBlock(int globalBlockX, int globalBlockY, int globalBlockZ, ve::blocks::BlockId blockId);

		/// Writes a block in world coordinates and marks affected chunks dirty.
		bool SetBlock(const glm::ivec3& position, ve::blocks::BlockId blockId);

		/** @return Snapshot of world size and chunk storage counts. */
		WorldMetrics Metrics() const noexcept;

		/** @return Monotonic counter bumped whenever block storage changes. */
		[[nodiscard]] std::uint64_t Revision() const noexcept;

		/** @return Monotonic counter bumped whenever chunk storage is replaced. */
		[[nodiscard]] std::uint64_t ChunkStorageRevision() const noexcept;

		/** @return Loaded chunks in storage order. */
		[[nodiscard]] std::span<const Chunk> Chunks() const noexcept;

		/** @return Changed chunk metadata tracked separately from chunk storage for persistence/streaming. */
		[[nodiscard]] std::span<const DirtyChunkMetadata> DirtyChunks() const noexcept;

		/** @return Events emitted since the previous drain. */
		std::vector<WorldEvent> DrainEvents();

		/** @param filter Event type filter. @return Matching events while leaving non-matching events queued. */
		std::vector<WorldEvent> DrainEvents(const WorldEventFilter& filter);

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
		/// Clears chunk storage and stale events before spawning a replacement world.
		void ResetChunkStorageForRespawn(int worldSizeChunks);

		/// Returns the chunk that contains a chunk-grid coordinate.
		Chunk* FindChunk(int chunkX, int chunkZ);

		/// Returns the chunk that contains a chunk-grid coordinate.
		const Chunk* FindChunk(int chunkX, int chunkZ) const;

		/// Marks neighbor chunks dirty when a changed local block touches a border.
		void MarkBorderNeighborsDirty(int chunkX, int chunkZ, int localX, int localZ);

		/// Marks a chunk dirty and mirrors its metadata into the world-owned dirty list.
		void MarkChunkDirty(Chunk& chunk);

		/// Mirrors dirty metadata for a chunk that has already been marked dirty.
		void RecordDirtyChunk(const Chunk& chunk);

		/// Collects neighboring chunks used to hide shared border faces.
		ve::world::mesh::NeighborChunks FindNeighborChunks(int chunkX, int chunkZ) const;

		/// Records a generated chunk event.
		void RecordChunkGenerated(int chunkX, int chunkZ);

		/// Records a block changed event.
		void RecordBlockChanged(const glm::ivec3& position, ve::blocks::BlockId previousBlockId, ve::blocks::BlockId newBlockId);

		/** @return Backend-owned mesh resource for one chunk, or null when the active backend is headless for chunk meshes. */
		std::unique_ptr<ve::rendering::RenderMesh> CreateChunkRenderMeshResource() const;

		/// Marks a generated chunk and its direct neighbors dirty.
		void MarkGeneratedChunkNeighborhoodDirty(int chunkX, int chunkZ);

		LevelSpawn _levelSpawn;
		ChunkList _chunks;
		std::vector<DirtyChunkMetadata> dirty_chunks_;
		std::vector<WorldEvent> _pendingEvents;
		const ve::rendering::RenderBackend* active_render_backend_;
		int _worldSize;
		std::uint64_t _revision;
		std::uint64_t _chunkStorageRevision;
	};
}
