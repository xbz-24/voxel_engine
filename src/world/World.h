#pragma once

#include "AsyncWorldGenerator.h"
#include "ChunkMeshPipeline.h"
#include "ChunkMeshRequest.h"
#include "LevelSpawn.h"
#include "WorldChunkStorage.h"
#include "WorldConfiguration.h"
#include "WorldEvent.h"
#include "WorldRenderRequest.h"

#include <glm/glm.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
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
	// Owns chunk storage and coordinates generation, meshing, rendering, and events.
	class World
	{
	public:
		explicit World(const WorldCreateInfo& createInfo);
		explicit World(std::size_t chunkCount);
		World(const World&) = delete;
		World& operator=(const World&) = delete;
		World(World&&) = delete;
		World& operator=(World&&) = delete;

		void SetRenderBackend(const ve::rendering::RenderBackend* renderBackend) noexcept;
		// Respawn keeps the previous grid intact when preparation throws.
		void SpawnFlatGrid(int worldSize);
		void SpawnFlatGrid(const FlatWorldSpawnSettings& settings);
		void SpawnEmptyGrid(const FlatWorldSpawnSettings& settings);
		bool ApplyGeneratedChunk(const generation::ChunkGenerationResult& result);
		void Draw(const WorldRenderRequest& request);
		ChunkRenderItemList ExtractVisibleChunks(const WorldRenderRequest& request) const;

		ve::blocks::BlockId GetBlock(int globalBlockX, int globalBlockY, int globalBlockZ) const;
		ve::blocks::BlockId GetBlock(const glm::ivec3& position) const;
		bool SetBlock(int globalBlockX, int globalBlockY, int globalBlockZ, ve::blocks::BlockId blockId);
		bool SetBlock(const glm::ivec3& position, ve::blocks::BlockId blockId);

		WorldMetrics Metrics() const noexcept;
		[[nodiscard]] std::uint64_t Revision() const noexcept;
		[[nodiscard]] std::uint64_t ChunkStorageRevision() const noexcept;
		[[nodiscard]] std::span<const Chunk> Chunks() const noexcept;
		[[nodiscard]] std::span<const DirtyChunkMetadata> DirtyChunks() const noexcept;
		std::vector<WorldEvent> DrainEvents();
		std::vector<WorldEvent> DrainEvents(const WorldEventFilter& filter);
		std::size_t PendingEventCount() const noexcept;

		std::optional<mesh::ChunkMeshBuildRequest> CaptureChunkMeshBuildRequest(int chunkX, int chunkZ) const;
		bool TryUploadChunkMeshOutput(mesh::ChunkMeshBuildOutput output);
		void UploadReadyChunkMeshes(mesh::ChunkMeshPipeline& meshPipeline);
		void ScheduleVisibleChunkMeshes(
			const ve::blocks::BlockRegistry& blockRegistry,
			mesh::ChunkMeshPipeline& meshPipeline,
			const glm::vec3& cameraPosition,
			int render_distance_chunks);

	private:
		static std::size_t EstimateLevelArenaBytes(const WorldCreateInfo& createInfo);
		void SpawnPreparedGrid(const FlatWorldSpawnSettings& settings, ChunkGenerationMode generation_mode);
		void ResetChunkStorageForRespawn(int worldSizeChunks) noexcept;
		Chunk* FindChunk(int chunkX, int chunkZ);
		const Chunk* FindChunk(int chunkX, int chunkZ) const;
		void MarkBorderNeighborsDirty(int chunkX, int chunkZ, int localX, int localZ);
		void MarkChunkDirty(Chunk& chunk);
		void RecordDirtyChunk(const Chunk& chunk);
		mesh::NeighborChunks FindNeighborChunks(int chunkX, int chunkZ) const;
		void RecordChunkGenerated(int chunkX, int chunkZ);
		void RecordBlockChanged(
			const glm::ivec3& position,
			ve::blocks::BlockId previousBlockId,
			ve::blocks::BlockId newBlockId);
		void MarkGeneratedChunkNeighborhoodDirty(int chunkX, int chunkZ);

		LevelSpawn _levelSpawn;
		ChunkList _chunks;
		std::vector<DirtyChunkMetadata> dirty_chunks_;
		std::vector<WorldEvent> _pendingEvents;
		const ve::rendering::RenderBackend* active_render_backend_;
		int _worldSize;
		std::uint64_t _revision;
		std::uint64_t _chunkStorageRevision;
		ChunkStoragePolicy chunk_storage_policy_;
	};
}
