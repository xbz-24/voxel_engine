#include "GreedyChunkMesher.h"

namespace ve::world::mesh
{
	ve::blocks::BlockId GreedyChunkMesher::ReadBlock(
		int local_block_x,
		int local_block_y,
		int local_block_z) const
	{
		if (local_block_y < 0 || local_block_y >= terrain::ChunkHeight) return ve::blocks::BlockId::Air;
		if (local_block_x < 0)
		{
			return neighbors_.west
				? neighbors_.west->GetBlock(terrain::ChunkWidth - 1, local_block_y, local_block_z)
				: ve::blocks::BlockId::Air;
		}
		if (local_block_x >= terrain::ChunkWidth)
		{
			return neighbors_.east
				? neighbors_.east->GetBlock(0, local_block_y, local_block_z)
				: ve::blocks::BlockId::Air;
		}
		if (local_block_z < 0)
		{
			return neighbors_.north
				? neighbors_.north->GetBlock(local_block_x, local_block_y, terrain::ChunkDepth - 1)
				: ve::blocks::BlockId::Air;
		}
		if (local_block_z >= terrain::ChunkDepth)
		{
			return neighbors_.south
				? neighbors_.south->GetBlock(local_block_x, local_block_y, 0)
				: ve::blocks::BlockId::Air;
		}
		return mesh_input_.GetBlock(local_block_x, local_block_y, local_block_z);
	}
}
