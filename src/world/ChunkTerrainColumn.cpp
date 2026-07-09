#include "ChunkTerrainColumn.h"

#include "ChunkTerrainMath.h"
#include "ChunkTerrainPalette.h"
#include "ChunkTerrainSurface.h"

#include <cstdint>

namespace ve::world::terrain::detail
{
	TerrainColumn BuildTerrainColumn(
		int chunk_coordinate_x,
		int chunk_coordinate_z,
		int local_block_x,
		int local_block_z,
		const ve::world::TerrainGenerationSettings& terrain_generation)
	{
		const float world_block_x = WorldBlockCoordinate(chunk_coordinate_x, local_block_x, ChunkWidth);
		const float world_block_z = WorldBlockCoordinate(chunk_coordinate_z, local_block_z, ChunkDepth);
		const int seed_pattern = TerrainSeedPattern(terrain_generation.terrainSeed);
		return {
			local_block_x,
			local_block_z,
			world_block_x,
			world_block_z,
			SurfaceHeight(world_block_x, world_block_z, terrain_generation),
			WeightedTerrainPattern(world_block_x, world_block_z, 3.0f, 11.0f, seed_pattern),
			WeightedTerrainPattern(world_block_x, world_block_z, 5.0f, 9.0f, seed_pattern)
		};
	}

	void FillTerrainColumn(
		const TerrainColumn& column,
		const ve::world::TerrainGenerationSettings& terrain_generation,
		BlockStorage& blocks)
	{
		const ve::world::TerrainBlockPalette block_palette = ResolveTerrainPalette(terrain_generation);
		for (int local_block_y = 0; local_block_y < ChunkHeight; ++local_block_y)
		{
			blocks[column.local_block_x][local_block_y][column.local_block_z] =
				BlockForDepth(column, local_block_y, block_palette, terrain_generation);
		}
	}
}
