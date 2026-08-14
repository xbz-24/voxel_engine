#include "WorldLoadDocumentTestSupport.h"

#include <doctest/doctest.h>

#include <array>
#include <string_view>

using namespace world_load_test;

TEST_CASE("strict world loading rejects missing files and malformed headers")
{
	CheckMissingRejected();
	for (const std::string_view contents : std::array<std::string_view, 5>{{
		"", "not-a-world 1\nsize 4\n", "voxel-world-config\nsize 4\n",
		"voxel-world-config one\nsize 4\n", "voxel-world-config 2\nsize 4\n" }})
	{
		CheckRejected(contents);
	}
}

TEST_CASE("strict world loading rejects unknown commands")
{
	CheckRejected(Document("size 4\nteleport 1 2 3\n"));
}

TEST_CASE("strict world loading rejects trailing record tokens")
{
	CheckRejected("voxel-world-config 1 extra\nsize 4\n");
	for (const std::string_view record : std::array<std::string_view, 3>{{
		"size 4 extra\n", "size 4\nset 1 2 3 3 extra\n",
		"size 4\nfill 1 2 3 4 5 6 3 extra\n" }})
	{
		CheckRejected(Document(record));
	}

	for (std::size_t index = 0; index < TerrainRecords.size(); ++index)
	{
		CheckRejected(TerrainDocument(
			index, std::string(TerrainRecords[index]) + " extra"));
	}
}
