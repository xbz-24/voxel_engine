#include "ChunkTerrain.h"

#include "ChunkDecorations.h"
#include "ChunkTerrainColumn.h"

namespace ve::world::terrain
{
	void GenerateChunkTerrain(
		int chunk_coordinate_x,
		int chunk_coordinate_z,
		const ve::world::TerrainGenerationSettings& terrain_generation,
		BlockStorage& blocks)
	{
		for (int local_block_x = 0; local_block_x < ChunkWidth; ++local_block_x)
		{
			for (int local_block_z = 0; local_block_z < ChunkDepth; ++local_block_z)
			{
				const detail::TerrainColumn column =
					detail::BuildTerrainColumn(
						chunk_coordinate_x,
						chunk_coordinate_z,
						local_block_x,
						local_block_z,
						terrain_generation);
				detail::FillTerrainColumn(column, terrain_generation, blocks);
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

	void GenerateChunkTerrain(int chunk_coordinate_x, int chunk_coordinate_z, BlockStorage& blocks)
	{
		GenerateChunkTerrain(chunk_coordinate_x, chunk_coordinate_z, ve::world::TerrainGenerationSettings{}, blocks);
	}
}
