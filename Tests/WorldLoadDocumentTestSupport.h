#pragma once

#include "WorldLoadTestSupport.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace world_load_test
{
	inline constexpr std::array<std::string_view, 5> TerrainRecords{{
		"terrain_seed 7",
		"base_surface_height 12",
		"terrain_generator flat",
		"terrain_biome desert",
		"terrain_palette 28 14 3 1 2 21 7 19"
	}};

	inline void AppendRecord(std::string& document, std::string_view record)
	{
		document.append(record);
		document.push_back('\n');
	}

	inline std::string TerrainDocument(
		std::size_t changed_index = TerrainRecords.size(),
		std::string_view replacement = {})
	{
		std::string document = Document("size 4\n");
		for (std::size_t index = 0; index < TerrainRecords.size(); ++index)
		{
			if (index == changed_index)
			{
				if (!replacement.empty()) AppendRecord(document, replacement);
			}
			else
			{
				AppendRecord(document, TerrainRecords[index]);
			}
		}
		return document;
	}

	inline std::string DuplicateTerrainDocument(std::size_t index)
	{
		std::string document = TerrainDocument();
		AppendRecord(document, TerrainRecords[index]);
		return document;
	}
}
