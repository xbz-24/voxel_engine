#include "World.h"

#include "RenderBackend.h"

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
			const std::size_t chunkBytes = chunkCount * sizeof(Chunk);
			const std::size_t debugProxyAndAlignmentPadding = 64u * 1024u;
			const std::size_t growthSlack = chunkBytes / 8u;
			return chunkBytes + debugProxyAndAlignmentPadding + growthSlack;
		}
	}

	/**
	 * Creates an empty world from explicit memory settings.
	 *
	 * @param createInfo Chunk capacity and arena sizing data.
	 */
	World::World(const WorldCreateInfo& createInfo)
		: World(createInfo.chunkCapacity)
	{
	}

	/**
	 * Creates an empty world with enough arena memory for the requested chunks.
	 *
	 * @param chunkCount Number of chunks to reserve in the level arena.
	 */
	World::World(std::size_t chunkCount)
		: _levelSpawn(EstimateWorldArenaBytes(chunkCount)),
		  _chunks(ChunkAllocator(&_levelSpawn.MemoryResource())),
		  active_render_backend_(nullptr),
		  _worldSize(0),
		  _revision(0)
	{
		_chunks.reserve(chunkCount);
	}

	void World::SetRenderBackend(const ve::rendering::RenderBackend* renderBackend) noexcept
	{
		active_render_backend_ = renderBackend;
	}

	std::unique_ptr<ve::rendering::RenderMesh> World::CreateChunkRenderMeshResource() const
	{
		return active_render_backend_ != nullptr ? active_render_backend_->CreateMeshResource() : nullptr;
	}

	/**
	 * Returns loaded chunk and capacity diagnostics.
	 *
	 * @return Snapshot of world size and chunk storage counts.
	 */
	WorldMetrics World::Metrics() const noexcept
	{
		std::size_t chunksNeedingMeshBuild = 0;
		std::size_t chunksWithQueuedMeshBuild = 0;
		for (const Chunk& chunk : _chunks)
		{
			if (chunk.NeedsMeshBuild()) chunksNeedingMeshBuild++;
			if (chunk.HasPendingMeshBuildReservation()) chunksWithQueuedMeshBuild++;
		}
		return WorldMetrics{
			_worldSize,
			_chunks.size(),
			_chunks.capacity(),
			chunksNeedingMeshBuild,
			chunksWithQueuedMeshBuild,
			0,
			0,
			0,
			_chunks.capacity() * sizeof(Chunk),
			_levelSpawn.MemoryCapacityBytes(),
			_levelSpawn.MemoryBytesUsed(),
			_pendingEvents.size()
		};
	}

	std::uint64_t World::Revision() const noexcept
	{
		return _revision;
	}
}
