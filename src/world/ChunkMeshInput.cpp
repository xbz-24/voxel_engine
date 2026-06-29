#include "ChunkMeshInput.h"

#include <cstddef>

namespace
{
	/// Returns true when a local block coordinate is inside one chunk.
	bool IsInsideChunk(int local_block_x, int local_block_y, int local_block_z)
	{
		return local_block_x >= 0 && local_block_x < ve::world::terrain::ChunkWidth &&
			local_block_y >= 0 && local_block_y < ve::world::terrain::ChunkHeight &&
			local_block_z >= 0 && local_block_z < ve::world::terrain::ChunkDepth;
	}

	/// Converts a 3D local coordinate into a flat storage index.
	std::size_t ToBlockIndex(int local_block_x, int local_block_y, int local_block_z)
	{
		return static_cast<std::size_t>(
			(local_block_x * ve::world::terrain::ChunkHeight + local_block_y) *
			ve::world::terrain::ChunkDepth + local_block_z);
	}
}

namespace ve::world::mesh
{
	/// Reads a local block from a flattened chunk snapshot.
	ve::blocks::BlockId ChunkMeshInput::GetBlock(int local_block_x, int local_block_y, int local_block_z) const
	{
		if (!IsInsideChunk(local_block_x, local_block_y, local_block_z))
		{
			return ve::blocks::BlockId::Air;
		}

		const std::size_t block_index = ToBlockIndex(local_block_x, local_block_y, local_block_z);
		if (block_index >= blocks.size())
		{
			return ve::blocks::BlockId::Air;
		}
		return blocks[block_index];
	}
}
