#include "ChunkTerrain.h"

#include "ChunkDecorations.h"
#include "ChunkTerrainColumn.h"

namespace ve::world::terrain
{
	void GenerateChunkTerrain(int chunk_x, int chunk_z, BlockStorage& blocks)
	{
		for (int local_block_x = 0; local_block_x < ChunkWidth; ++local_block_x)
		{
			for (int local_block_z = 0; local_block_z < ChunkDepth; ++local_block_z)
			{
				const detail::TerrainColumn column =
					detail::BuildTerrainColumn(chunk_x, chunk_z, local_block_x, local_block_z);
				detail::FillTerrainColumn(column, blocks);
				decorations::ApplySurfaceDecorations(
					column.local_block_x,
					column.local_block_z,
					column.surface_y,
					column.world_block_x,
					column.world_block_z,
					column.surface_block_pattern,
					blocks);
			}
		}
	}
}
