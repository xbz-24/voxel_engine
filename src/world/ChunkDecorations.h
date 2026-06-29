#pragma once

#include "ChunkTerrain.h"

namespace ve::world::terrain::decorations
{
	/**
	 * Adds surface decorations such as trees, pumpkins, work blocks, and small ruins.
	 *
	 * @param local_block_x Local block X coordinate.
	 * @param local_block_z Local block Z coordinate.
	 * @param surface_y Top terrain block height.
	 * @param world_block_x World X coordinate.
	 * @param world_block_z World Z coordinate.
	 * @param surface_block_pattern Stable procedural pattern value.
	 * @param blocks Chunk storage receiving decorations.
	 */
	void ApplySurfaceDecorations(int local_block_x, int local_block_z, int surface_y, float world_block_x, float world_block_z, int surface_block_pattern, BlockStorage& blocks);
}
