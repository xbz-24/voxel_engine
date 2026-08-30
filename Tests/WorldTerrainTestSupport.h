#pragma once

#include "ChunkTerrain.h"

[[nodiscard]] inline int HighestSolidBlockY(
	const ve::world::terrain::BlockStorage& blocks,
	int x,
	int z)
{
	for (int y = ve::world::terrain::ChunkHeight - 1; y >= 0; --y)
	{
		if (blocks[x][y][z] != ve::blocks::BlockId::Air)
		{
			return y;
		}
	}
	return -1;
}
