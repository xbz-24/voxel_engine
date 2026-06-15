#include "ChunkMeshInput.h"

#include <cstddef>

namespace
{
	/// Returns true when a local block coordinate is inside one chunk.
	bool IsInsideChunk(int x, int y, int z)
	{
		return x >= 0 && x < ve::world::terrain::ChunkWidth &&
			y >= 0 && y < ve::world::terrain::ChunkHeight &&
			z >= 0 && z < ve::world::terrain::ChunkDepth;
	}

	/// Converts a 3D local coordinate into a flat storage index.
	std::size_t ToBlockIndex(int x, int y, int z)
	{
		return static_cast<std::size_t>((x * ve::world::terrain::ChunkHeight + y) *
			ve::world::terrain::ChunkDepth + z);
	}
}

namespace ve::world::mesh
{
	/// Reads a local block from a flattened chunk snapshot.
	ve::blocks::BlockId ChunkMeshInput::GetBlock(int x, int y, int z) const
	{
		if (!IsInsideChunk(x, y, z) || ToBlockIndex(x, y, z) >= blocks.size())
		{
			return ve::blocks::BlockId::Air;
		}
		return blocks[ToBlockIndex(x, y, z)];
	}
}
