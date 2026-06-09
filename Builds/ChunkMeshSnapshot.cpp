#include "ChunkMeshSnapshot.h"

#include "Chunk.h"

#include <algorithm>

namespace ve::world::mesh
{
	/// Creates a mesh input view over this owned snapshot.
	ChunkMeshInput ChunkMeshSnapshot::CreateInput() const noexcept
	{
		return ChunkMeshInput{ chunkX, chunkZ, blocks };
	}

	/// Copies a chunk's current block data into an immutable snapshot.
	ChunkMeshSnapshot CaptureChunkMeshSnapshot(const Chunk& chunk)
	{
		const ChunkMeshInput chunkInput = chunk.CreateMeshInput();
		ChunkMeshSnapshot snapshot{ chunkInput.chunkX, chunkInput.chunkZ, {} };
		std::copy(chunkInput.blocks.begin(), chunkInput.blocks.end(), snapshot.blocks.begin());
		return snapshot;
	}
}
