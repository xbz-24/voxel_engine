#include "Chunk.h"

#include "ChunkTerrain.h"

#include <algorithm>

using ve::blocks::BlockId;

/**
 * Fills block data using procedural terrain generation.
 */
void Chunk::Generate(const ve::world::TerrainGenerationSettings& terrain_generation)
{
	ve::world::terrain::GenerateChunkTerrain(chunk_x_, chunk_z_, terrain_generation, blocks_);
	is_generated_ = true;
	has_procedural_terrain_ = true;
	has_authored_edits_ = false;
	is_mesh_build_queued_ = false;
	MarkDirty();
}

/**
 * Replaces all local block data with generated terrain.
 *
 * @param generated_blocks Flat block data in chunk-local x/y/z order.
 * @return True when the input size matched this chunk.
 */
bool Chunk::ReplaceBlocks(std::span<const BlockId> generated_blocks)
{
	if (generated_blocks.size() != ve::world::terrain::ChunkBlockCount) return false;
	std::copy(generated_blocks.begin(), generated_blocks.end(), &blocks_[0][0][0]);
	is_generated_ = true;
	has_procedural_terrain_ = true;
	has_authored_edits_ = false;
	is_mesh_build_queued_ = false;
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
	return is_generated_;
}

bool Chunk::HasProceduralTerrain() const noexcept
{
	return has_procedural_terrain_;
}

bool Chunk::HasAuthoredEdits() const noexcept
{
	return has_authored_edits_;
}

ChunkContentProvenance Chunk::Provenance() const noexcept
{
	if (has_procedural_terrain_ && has_authored_edits_) return ChunkContentProvenance::ProceduralTerrainWithAuthoredEdits;
	if (has_procedural_terrain_) return ChunkContentProvenance::ProceduralTerrain;
	if (has_authored_edits_) return ChunkContentProvenance::AuthoredEdits;
	return ChunkContentProvenance::Empty;
}
