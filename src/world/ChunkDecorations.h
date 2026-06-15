#pragma once

#include "ChunkTerrain.h"

namespace ve::world::terrain::decorations
{
	/**
	 * Adds surface decorations such as trees, pumpkins, work blocks, and small ruins.
	 *
	 * @param x Local block X coordinate.
	 * @param z Local block Z coordinate.
	 * @param surfaceHeight Top terrain block height.
	 * @param globalX World X coordinate.
	 * @param globalZ World Z coordinate.
	 * @param surfacePattern Stable procedural pattern value.
	 * @param blocks Chunk storage receiving decorations.
	 */
	void ApplySurfaceDecorations(int x, int z, int surfaceHeight, float globalX, float globalZ, int surfacePattern, BlockStorage& blocks);
}
