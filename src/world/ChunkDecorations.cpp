#include "ChunkDecorations.h"

#include "ChunkDecorationRules.h"

namespace ve::world::terrain::decorations
{
	void ApplySurfaceDecorations(
		int local_block_x,
		int local_block_z,
		int surface_y,
		float world_block_x,
		float world_block_z,
		int surface_block_pattern,
		BlockStorage& blocks)
	{
		detail::AddStoneDetails(local_block_x, local_block_z, surface_y, world_block_x, world_block_z, blocks);
		if (detail::TryAddTreeDecoration(
				local_block_x,
				local_block_z,
				surface_y,
				world_block_x,
				world_block_z,
				blocks))
		{
			return;
		}
		detail::TryAddSurfaceDecoration(local_block_x, local_block_z, surface_y, surface_block_pattern, blocks);
	}
}
