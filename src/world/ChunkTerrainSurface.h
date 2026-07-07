#pragma once

#include "ChunkTerrainColumn.h"
#include "WorldConfiguration.h"

namespace ve::world::terrain::detail
{
	[[nodiscard]] int SurfaceHeight(
		float world_block_x,
		float world_block_z,
		const ve::world::TerrainGenerationSettings& terrain_generation);

	[[nodiscard]] ve::blocks::BlockId BlockForDepth(
		const TerrainColumn& column,
		int local_block_y,
		const ve::world::TerrainBlockPalette& block_palette,
		const ve::world::TerrainGenerationSettings& terrain_generation);
}
