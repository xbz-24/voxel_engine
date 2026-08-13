#include "World.h"

#include "RenderBackend.h"

#include <memory_resource>

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

		std::pmr::memory_resource* ChunkStorageResource(
			LevelSpawn& level_spawn,
			ChunkStoragePolicy chunk_storage_policy) noexcept
		{
			if (chunk_storage_policy == ChunkStoragePolicy::GrowOnDemand)
			{
				return std::pmr::get_default_resource();
			}
			return &level_spawn.MemoryResource();
		}
	}

	std::size_t World::EstimateLevelArenaBytes(const WorldCreateInfo& createInfo)
	{
		if (createInfo.chunkStoragePolicy == ChunkStoragePolicy::GrowOnDemand)
		{
			return 0U;
		}
		return EstimateWorldArenaBytes(createInfo.chunkCapacity);
	}

	/**
	 * Creates an empty world from explicit memory settings.
	 *
	 * @param createInfo Chunk capacity and arena sizing data.
	 */
	World::World(const WorldCreateInfo& createInfo)
		: _levelSpawn(EstimateLevelArenaBytes(createInfo)),
		  _chunks(ChunkAllocator(ChunkStorageResource(_levelSpawn, createInfo.chunkStoragePolicy))),
		  active_render_backend_(nullptr),
		  _worldSize(0),
		  _revision(0),
		  _chunkStorageRevision(0),
		  chunk_storage_policy_(createInfo.chunkStoragePolicy)
	{
		if (chunk_storage_policy_ == ChunkStoragePolicy::FixedReserve)
		{
			_chunks.reserve(createInfo.chunkCapacity);
		}
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
		  _revision(0),
		  _chunkStorageRevision(0),
		  chunk_storage_policy_(ChunkStoragePolicy::FixedReserve)
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

}
