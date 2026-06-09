#pragma once

#include "BlockRegistry.h"
#include "ChunkMeshSnapshot.h"
#include "ChunkMeshTypes.h"

#include <optional>

namespace ve::world::mesh
{
	struct ChunkMeshBuildRequest
	{
		ChunkMeshSnapshot chunk;
		std::optional<ChunkMeshSnapshot> west;
		std::optional<ChunkMeshSnapshot> east;
		std::optional<ChunkMeshSnapshot> north;
		std::optional<ChunkMeshSnapshot> south;
	};

	struct ChunkMeshBuildOutput
	{
		int chunkX;
		int chunkZ;
		ChunkMeshBuildResult mesh;
	};

	/**
	 * Builds CPU mesh output from owned chunk snapshots.
	 *
	 * @param request Center chunk and optional neighbor snapshots.
	 * @param blockRegistry Registry used for textures and solidity checks.
	 * @return Chunk coordinates plus mesh data ready for main-thread upload.
	 */
	ChunkMeshBuildOutput BuildChunkMeshOutput(const ChunkMeshBuildRequest& request, const ve::blocks::BlockRegistry& blockRegistry);
}
