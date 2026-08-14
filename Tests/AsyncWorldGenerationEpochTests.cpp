#include <doctest/doctest.h>

#include "AsyncWorldGenerator.h"

#include <chrono>
#include <cstdint>
#include <thread>

namespace
{
	ve::core::DynamicArray<ve::world::generation::ChunkGenerationResult> WaitForGeneratedChunks(
		ve::world::generation::AsyncWorldGenerator& generator)
	{
		const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
		do
		{
			auto completed = generator.DrainCompletedChunks();
			if (!completed.empty()) return completed;
			std::this_thread::yield();
		} while (std::chrono::steady_clock::now() < deadline);
		return generator.DrainCompletedChunks();
	}
}

TEST_CASE("async world grid generation propagates its chunk storage epoch")
{
	constexpr std::uint64_t expected_epoch = 0x123456789abcdef0ULL;
	ve::world::generation::AsyncWorldGenerator generator(1);
	generator.RequestGrid(ve::world::FlatWorldSpawnSettings{ 1 }, expected_epoch);

	const auto completed = WaitForGeneratedChunks(generator);
	REQUIRE(completed.size() == 1);
	CHECK(completed.front().chunkCoordinateX == 0);
	CHECK(completed.front().chunkCoordinateZ == 0);
	CHECK(completed.front().chunkStorageRevision == expected_epoch);
	CHECK(generator.PendingTaskCount() == 0);
}
