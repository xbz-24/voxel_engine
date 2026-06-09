#include "Chunk.h"

#include "ChunkTerrain.h"

#include <algorithm>

using ve::blocks::BlockId;

/**
 * Fills block data using procedural terrain generation.
 */
void Chunk::Generate()
{
	ve::world::terrain::GenerateChunkTerrain(_chunkX, _chunkZ, blocks);
	_isGenerated = true;
	MarkDirty();
}

/**
 * Replaces all local block data with generated terrain.
 *
 * @param generatedBlocks Flat block data in chunk-local x/y/z order.
 * @return True when the input size matched this chunk.
 */
bool Chunk::ReplaceBlocks(std::span<const BlockId> generatedBlocks)
{
	if (generatedBlocks.size() != ve::world::terrain::ChunkBlockCount) return false;
	std::copy(generatedBlocks.begin(), generatedBlocks.end(), &blocks[0][0][0]);
	_isGenerated = true;
	MarkDirty();
	return true;
}

/**
 * Reports whether this chunk contains generated terrain data.
 *
 * @return True when terrain data has been generated or loaded.
 */
bool Chunk::IsGenerated() const noexcept
{
	return _isGenerated;
}
