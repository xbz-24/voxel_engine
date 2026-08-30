#include <doctest/doctest.h>

#include "ChunkTerrain.h"
#include "WorldViewRange.h"

#include <limits>

namespace
{
	void CheckFullWorldRange(const glm::vec3& camera_position)
	{
		const ve::world::ChunkViewRange range = ve::world::BuildChunkViewRange(
			camera_position,
			64,
			std::numeric_limits<int>::max());

		CHECK(range.minChunkX == 0);
		CHECK(range.maxChunkX == 63);
		CHECK(range.minChunkZ == 0);
		CHECK(range.maxChunkZ == 63);
	}
}

TEST_CASE("maximum render distance safely covers the world from positive chunks")
{
	constexpr float ChunkWidth = static_cast<float>(ve::world::terrain::ChunkWidth);
	CheckFullWorldRange({ ChunkWidth, 0.0f, ChunkWidth });
}

TEST_CASE("maximum render distance safely covers the world from negative chunks")
{
	constexpr float ChunkWidth = static_cast<float>(ve::world::terrain::ChunkWidth);
	CheckFullWorldRange({ -ChunkWidth - 1.0f, 0.0f, -ChunkWidth - 1.0f });
}

TEST_CASE("ordinary render distance preserves the inclusive chunk range")
{
	constexpr float ChunkWidth = static_cast<float>(ve::world::terrain::ChunkWidth);
	const ve::world::ChunkViewRange range = ve::world::BuildChunkViewRange(
		{ 3.0f * ChunkWidth, 0.0f, 4.0f * ChunkWidth },
		8,
		2);

	CHECK(range.minChunkX == 1);
	CHECK(range.maxChunkX == 5);
	CHECK(range.minChunkZ == 2);
	CHECK(range.maxChunkZ == 6);
}

TEST_CASE("camera ranges beyond either world edge remain empty")
{
	constexpr float ChunkWidth = static_cast<float>(ve::world::terrain::ChunkWidth);
	const ve::world::ChunkViewRange beyond_right = ve::world::BuildChunkViewRange(
		{ 70.0f * ChunkWidth, 0.0f, 0.0f }, 64, 2);
	const ve::world::ChunkViewRange beyond_left = ve::world::BuildChunkViewRange(
		{ -4.0f * ChunkWidth, 0.0f, 0.0f }, 64, 2);

	CHECK_FALSE(ve::world::HasChunks(beyond_right));
	CHECK_FALSE(ve::world::HasChunks(beyond_left));
}
