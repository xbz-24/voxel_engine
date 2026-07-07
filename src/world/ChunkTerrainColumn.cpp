#include "ChunkTerrainColumn.h"

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
		const float world_block_x = static_cast<float>(local_block_x + (chunk_coordinate_x * ChunkWidth));
		const float world_block_z = static_cast<float>(local_block_z + (chunk_coordinate_z * ChunkDepth));
		const int seed_pattern = static_cast<int>(terrain_generation.terrainSeed % 8191U);
		return {
			local_block_x,
			local_block_z,
			world_block_x,
			world_block_z,
			SurfaceHeight(world_block_x, world_block_z, terrain_generation),
			static_cast<int>(world_block_x * 3.0f + world_block_z * 11.0f) + seed_pattern,
			static_cast<int>(world_block_x * 5.0f + world_block_z * 9.0f) + seed_pattern
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
