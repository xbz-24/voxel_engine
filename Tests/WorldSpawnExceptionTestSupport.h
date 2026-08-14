#pragma once

#include <doctest/doctest.h>

#include "RenderBackend.h"
#include "World.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace ve::tests
{
	inline constexpr int SentinelBlockX = 0;
	inline constexpr int SentinelBlockY = 64;
	inline constexpr int SentinelBlockZ = 0;
	inline constexpr ve::blocks::BlockId SentinelBlock = ve::blocks::BlockId::Stone;

	class ThrowingMeshCreationBackend final : public ve::rendering::RenderBackend
	{
	public:
		explicit ThrowingMeshCreationBackend(int failure_call) noexcept
			: failure_call_(failure_call)
		{
		}

		[[nodiscard]] ve::rendering::GraphicsApi Api() const noexcept override
		{
			return ve::rendering::GraphicsApi::OpenGLCompatibility;
		}

		[[nodiscard]] ve::rendering::RenderBackendCapabilities Capabilities() const noexcept override
		{
			return ve::rendering::RenderBackendCapabilities{ .is_available = true };
		}

		[[nodiscard]] std::unique_ptr<ve::rendering::RenderMesh> CreateMeshResource() const override
		{
			++call_count_;
			if (call_count_ == failure_call_) throw std::runtime_error("mesh creation failure");
			return nullptr;
		}

		[[nodiscard]] int CallCount() const noexcept { return call_count_; }

	private:
		int failure_call_ = 0;
		mutable int call_count_ = 0;
	};

	struct WorldSnapshot
	{
		int world_size_chunks = 0;
		std::size_t loaded_chunk_count = 0;
		std::size_t reserved_chunk_capacity = 0;
		std::size_t level_arena_used_bytes = 0;
		std::size_t pending_event_count = 0;
		std::size_t dirty_chunk_count = 0;
		std::uint64_t revision = 0;
		std::uint64_t chunk_storage_revision = 0;
		ve::blocks::BlockId sentinel_block = ve::blocks::BlockId::Air;

		[[nodiscard]] friend bool operator==(const WorldSnapshot&, const WorldSnapshot&) = default;
	};

	inline WorldSnapshot CaptureWorldSnapshot(const ve::world::World& world)
	{
		const ve::world::WorldMetrics metrics = world.Metrics();
		return WorldSnapshot{
			metrics.worldSizeInChunks,
			metrics.loadedChunkCount,
			metrics.reservedChunkCapacity,
			metrics.levelArenaUsedBytes,
			metrics.pendingWorldEventCount,
			world.DirtyChunks().size(),
			world.Revision(),
			world.ChunkStorageRevision(),
			world.GetBlock(SentinelBlockX, SentinelBlockY, SentinelBlockZ)
		};
	}

	inline void SeedWorldForRespawnFailure(ve::world::World& world)
	{
		world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });
		REQUIRE(world.SetBlock(SentinelBlockX, SentinelBlockY, SentinelBlockZ, SentinelBlock));
	}
}
