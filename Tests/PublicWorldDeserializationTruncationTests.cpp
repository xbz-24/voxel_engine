#include "WorldLoadDocumentTestSupport.h"

#include <doctest/doctest.h>

#include <array>
#include <string_view>

using namespace world_load_test;

TEST_CASE("strict world loading rejects truncated scalar and edit records")
{
	for (const std::string_view record : std::array<std::string_view, 3>{{
		"size\n", "size 4\nset 1 2 3\n", "size 4\nfill -1 -2 -3 4 5 6\n" }})
	{
		CheckRejected(Document(record));
	}
}

TEST_CASE("strict world loading rejects every truncated terrain record")
{
	constexpr std::array<std::string_view, 5> truncated{{
		"terrain_seed",
		"base_surface_height",
		"terrain_generator",
		"terrain_biome",
		"terrain_palette 28 14 3 1 2 21 7"
	}};
	for (std::size_t index = 0; index < truncated.size(); ++index)
	{
		CheckRejected(TerrainDocument(index, truncated[index]));
	}
}

TEST_CASE("truncated maximum-coordinate fill never appends an edit")
{
	const TemporaryWorldPath path(Document("size 4\nfill 2147483647\n"));
	voxel::WorldConfig output = SentinelWorld();
	const std::size_t original_edit_count = output.edits.size();
	CHECK_FALSE(voxel::TryLoadWorldConfig(path.Get().string(), output));
	CHECK(output.edits.size() == original_edit_count);
	CHECK(SameWorld(output, SentinelWorld()));
}
