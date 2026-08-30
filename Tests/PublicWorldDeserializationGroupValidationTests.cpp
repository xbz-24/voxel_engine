#include "WorldLoadDocumentTestSupport.h"

#include <doctest/doctest.h>

using namespace world_load_test;

TEST_CASE("strict world loading requires exactly one size record")
{
	CheckRejected("voxel-world-config 1\n");
	CheckRejected(Document("size 4\nsize 5\n"));
}

TEST_CASE("strict world loading rejects partial terrain groups")
{
	for (std::size_t missing_index = 0;
		missing_index < TerrainRecords.size(); ++missing_index)
	{
		CAPTURE(missing_index);
		CheckRejected(TerrainDocument(missing_index));
	}
}

TEST_CASE("strict world loading rejects duplicate terrain singletons")
{
	for (std::size_t duplicate_index = 0;
		duplicate_index < TerrainRecords.size(); ++duplicate_index)
	{
		CAPTURE(duplicate_index);
		CheckRejected(DuplicateTerrainDocument(duplicate_index));
	}
}
