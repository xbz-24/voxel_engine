#pragma once

#include "Chunk.h"

#include <cstdint>
#include <memory_resource>
#include <vector>

namespace ve::world
{
	using ChunkAllocator = std::pmr::polymorphic_allocator<Chunk>;
	using ChunkList = std::vector<Chunk, ChunkAllocator>;

	struct DirtyChunkMetadata
	{
		int chunk_x = 0;
		int chunk_z = 0;
		std::uint64_t mesh_revision = 0;
		bool has_authored_edits = false;
	};
}
