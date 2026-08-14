#pragma once

#include <span>

class Chunk;

namespace ve::world::mesh
{
	/** Clears reservations after a worker failure could not publish an exact outcome. */
	void CancelPendingChunkMeshReservations(std::span<Chunk> chunks) noexcept;
}
