#pragma once

#include "Block.h"
#include "WorldConfiguration.h"

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
	 * @param chunk_x Chunk-grid X coordinate.
	 * @param chunk_z Chunk-grid Z coordinate.
	 * @param terrain_generation Settings used to shape terrain and block palette.
	 * @param blocks Storage filled with generated block ids.
	 */
	void GenerateChunkTerrain(
		int chunk_x,
		int chunk_z,
		const ve::world::TerrainGenerationSettings& terrain_generation,
		BlockStorage& blocks);

	/**
	 * Fills chunk block storage with default procedural terrain.
	 *
	 * @param chunk_x Chunk-grid X coordinate.
	 * @param chunk_z Chunk-grid Z coordinate.
	 * @param blocks Storage filled with generated block ids.
	 */
	void GenerateChunkTerrain(int chunk_x, int chunk_z, BlockStorage& blocks);
}
