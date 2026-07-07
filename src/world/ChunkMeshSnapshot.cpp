#include "ChunkMeshSnapshot.h"

#include "Chunk.h"

#include <algorithm>

namespace ve::world::mesh
{
	/// Creates a mesh input view over this owned snapshot.
	ChunkMeshInput ChunkMeshSnapshot::CreateInput() const noexcept
	{
		return ChunkMeshInput{ chunkX, chunkZ, std::span<const ve::blocks::BlockId>(blocks.data(), blocks.size()) };
	}

	/// Copies a chunk's current block data into an immutable snapshot.
	ChunkMeshSnapshot CaptureChunkMeshSnapshot(const Chunk& chunk)
	{
		const ChunkMeshInput chunkInput = chunk.CreateMeshInput();
		ChunkMeshSnapshot snapshot{ chunkInput.chunkX, chunkInput.chunkZ, chunk.MeshRevision() };
		snapshot.blocks.assign(chunkInput.blocks.begin(), chunkInput.blocks.end());
		return snapshot;
	}
}
