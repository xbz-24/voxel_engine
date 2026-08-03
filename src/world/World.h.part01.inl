
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

	// Owns storage, generation handoff, meshing handoff, and world events; roadmap tracks the headless split.
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
		// Streaming storage exists; non-square world bounds are tracked in the architecture roadmap.

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
