#include "ChunkMeshFailureRecovery.h"

#include "Chunk.h"

namespace ve::world::mesh
{
	void CancelPendingChunkMeshReservations(std::span<Chunk> chunks) noexcept
	{
		for (Chunk& chunk : chunks) chunk.CancelMeshBuildReservation();
	}
}
