#pragma once

#include "Block.h"

namespace ve::world::terrain
{
	constexpr int ChunkWidth = 16;
	constexpr int ChunkHeight = 128;
	constexpr int ChunkDepth = 16;
	inline constexpr int ChunkBlockCount = ChunkWidth * ChunkHeight * ChunkDepth;
	using BlockStorage = ve::blocks::BlockId[ChunkWidth][ChunkHeight][ChunkDepth];

	/**
	 * Fills chunk block storage with procedural terrain.
	 *
	 * @param chunkX Chunk-grid X coordinate.
	 * @param chunkZ Chunk-grid Z coordinate.
	 * @param blocks Storage filled with generated block ids.
	 */
	void GenerateChunkTerrain(int chunkX, int chunkZ, BlockStorage& blocks);
}
