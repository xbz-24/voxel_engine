#pragma once

#include "ChunkMeshInput.h"
#include "CoreTypes.h"

#include <cstdint>

class Chunk;

namespace ve::world::mesh
{
	struct ChunkMeshSnapshot
	{
		int chunkX;
		int chunkZ;
		std::uint64_t revision;
		ve::core::DynamicArray<ve::blocks::BlockId> blocks;

		/**
		 * Creates a mesh input view over this owned snapshot.
		 *
		 * @return Read-only mesh input valid while this snapshot is alive.
		 */
		ChunkMeshInput CreateInput() const noexcept;
	};

	/**
	 * Copies a chunk's current block data into an immutable snapshot.
	 *
	 * @param chunk Source chunk read on the game thread.
	 * @return Owned snapshot safe to move into a background task.
	 */
	ChunkMeshSnapshot CaptureChunkMeshSnapshot(const Chunk& chunk);
}
