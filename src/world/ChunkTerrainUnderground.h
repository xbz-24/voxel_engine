#pragma once

#include "Block.h"
#include "WorldConfiguration.h"

namespace ve::world::terrain::detail
{
	[[nodiscard]] ve::blocks::BlockId UndergroundBlock(
		int local_block_y,
		float world_block_x,
		float world_block_z,
		const ve::world::TerrainBlockPalette& block_palette,
		const ve::world::TerrainGenerationSettings& terrain_generation);
}
