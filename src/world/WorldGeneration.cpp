#include "World.h"

namespace ve::world
{
	/// Applies generated terrain data to a loaded chunk.
	bool World::ApplyGeneratedChunk(const generation::ChunkGenerationResult& result)
	{
		// TODO: Preserve generation provenance so save files can distinguish authored edits from procedural terrain.
		Chunk* chunk = FindChunk(result.chunk_x, result.chunk_z);
		if (!chunk || !chunk->ReplaceBlocks(result.blocks)) return false;
		++_revision;
		MarkGeneratedChunkNeighborhoodDirty(result.chunk_x, result.chunk_z);
		RecordChunkGenerated(result.chunk_x, result.chunk_z);
		return true;
	}

	/// Marks a generated chunk and its direct neighbors dirty.
	void World::MarkGeneratedChunkNeighborhoodDirty(int chunkX, int chunkZ)
	{
		// TODO: Generalize dirty-region expansion when chunk mesh dependencies grow beyond direct cardinal neighbors.
		if (Chunk* chunk = FindChunk(chunkX, chunkZ)) chunk->MarkDirty();
		if (Chunk* west = FindChunk(chunkX - 1, chunkZ)) west->MarkDirty();
		if (Chunk* east = FindChunk(chunkX + 1, chunkZ)) east->MarkDirty();
		if (Chunk* north = FindChunk(chunkX, chunkZ - 1)) north->MarkDirty();
		if (Chunk* south = FindChunk(chunkX, chunkZ + 1)) south->MarkDirty();
	}
}
