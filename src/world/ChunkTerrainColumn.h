#pragma once

#include "ChunkTerrain.h"

namespace ve::world::terrain::detail
{
	struct TerrainColumn
	{
		int local_block_x = 0;
		int local_block_z = 0;
		float world_block_x = 0.0f;
		float world_block_z = 0.0f;
		int surface_y = 0;
		int ground_patch_pattern = 0;
		int surface_block_pattern = 0;
	};

	[[nodiscard]] TerrainColumn BuildTerrainColumn(
		int chunk_x,
		int chunk_z,
		int local_block_x,
		int local_block_z);

	void FillTerrainColumn(const TerrainColumn& column, BlockStorage& blocks);
}
