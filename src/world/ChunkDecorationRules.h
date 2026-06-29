#pragma once

#include "ChunkTerrain.h"

namespace ve::world::terrain::decorations::detail
{
	void AddStoneDetails(
		int local_block_x,
		int local_block_z,
		int surface_y,
		float world_block_x,
		float world_block_z,
		BlockStorage& blocks);

	bool TryAddTreeDecoration(
		int local_block_x,
		int local_block_z,
		int surface_y,
		float world_block_x,
		float world_block_z,
		BlockStorage& blocks);

	bool TryAddSurfaceDecoration(
		int local_block_x,
		int local_block_z,
		int surface_y,
		int surface_block_pattern,
		BlockStorage& blocks);
}
