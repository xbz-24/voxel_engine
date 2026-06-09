#pragma once

#include "Block.h"
#include "ChunkTerrain.h"

#include <span>

namespace ve::world::mesh
{
	inline constexpr int ChunkBlockCount = terrain::ChunkBlockCount;

	struct ChunkMeshInput
	{
		int chunkX;
		int chunkZ;
		std::span<const ve::blocks::BlockId> blocks;

		/**
		 * Reads a local block from a flattened chunk snapshot.
		 *
		 * @param x Local block X coordinate.
		 * @param y Local block Y coordinate.
		 * @param z Local block Z coordinate.
		 * @return Block id, or Air when the coordinate is outside the chunk.
		 */
		ve::blocks::BlockId GetBlock(int x, int y, int z) const;
	};

	struct NeighborMeshInputs
	{
		const ChunkMeshInput* west;
		const ChunkMeshInput* east;
		const ChunkMeshInput* north;
		const ChunkMeshInput* south;
	};
}
