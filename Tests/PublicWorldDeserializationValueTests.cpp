#include "WorldLoadDocumentTestSupport.h"

#include <doctest/doctest.h>

#include <array>
#include <sstream>
#include <string>

using namespace world_load_test;

namespace
{
	std::string InvalidPaletteDocument(std::size_t invalid_index, int invalid_value)
	{
		constexpr std::array<int, 8> valid{{28, 14, 3, 1, 2, 21, 7, 19}};
		std::ostringstream record;
		record << "terrain_palette";
		for (std::size_t index = 0; index < valid.size(); ++index)
		{
			record << ' ' << (index == invalid_index ? invalid_value : valid[index]);
		}
		return TerrainDocument(4, record.str());
	}
}

TEST_CASE("strict world loading rejects invalid terrain scalar values")
{
	CheckRejected(TerrainDocument(0, "terrain_seed -1"));
	CheckRejected(TerrainDocument(0, "terrain_seed 4294967296"));
	CheckRejected(TerrainDocument(2, "terrain_generator caves"));
	CheckRejected(TerrainDocument(3, "terrain_biome jungle"));
}

TEST_CASE("strict world loading rejects signed integer overflow")
{
	CheckRejected(Document("size 2147483648\n"));
	CheckRejected(TerrainDocument(1, "base_surface_height -2147483649"));
	CheckRejected(Document("size 4\nset 2147483648 0 0 3\n"));
	CheckRejected(Document("size 4\nfill -2147483649 0 0 1 1 1 3\n"));
}

TEST_CASE("strict world loading rejects invalid blocks in every record kind")
{
	for (const int invalid_block : {-1, 255})
	{
		CAPTURE(invalid_block);
		for (std::size_t index = 0; index < 8; ++index)
		{
			CAPTURE(index);
			CheckRejected(InvalidPaletteDocument(index, invalid_block));
		}
		CheckRejected(Document(
			"size 4\nset 1 2 3 " + std::to_string(invalid_block) + "\n"));
		CheckRejected(Document(
			"size 4\nfill 1 2 3 4 5 6 " + std::to_string(invalid_block) + "\n"));
	}
}
