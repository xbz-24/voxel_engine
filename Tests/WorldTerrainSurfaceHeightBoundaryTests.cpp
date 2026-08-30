#include <doctest/doctest.h>

#include "ChunkTerrainSurface.h"

#include <limits>

namespace
{
	int ProceduralSurfaceHeight(float world_x, float world_z, int base_height)
	{
		ve::world::TerrainGenerationSettings settings{};
		settings.baseSurfaceHeight = base_height;
		settings.terrainGenerator = ve::world::TerrainGeneratorKind::Procedural;
		return ve::world::terrain::detail::SurfaceHeight(world_x, world_z, settings);
	}

	void CheckInsideChunkHeight(int surface_height)
	{
		CHECK(surface_height >= 1);
		CHECK(surface_height <= ve::world::terrain::ChunkHeight - 2);
	}
}

TEST_CASE("procedural surface height saturates an overflowing maximum base")
{
	const int surface_height = ProceduralSurfaceHeight(
		0.0f, 0.0f, std::numeric_limits<int>::max());

	CheckInsideChunkHeight(surface_height);
	CHECK(surface_height == ve::world::terrain::ChunkHeight - 2);
}

TEST_CASE("procedural surface height saturates an overflowing minimum base")
{
	const int surface_height = ProceduralSurfaceHeight(
		0.0f, 58.0f, std::numeric_limits<int>::lowest());

	CheckInsideChunkHeight(surface_height);
	CHECK(surface_height == 1);
}

TEST_CASE("procedural surface height preserves an ordinary terrain sample")
{
	const int surface_height = ProceduralSurfaceHeight(0.0f, 0.0f, 40);

	CheckInsideChunkHeight(surface_height);
	CHECK(surface_height == 57);
}
