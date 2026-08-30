#pragma once

#include "ChunkMeshTypes.h"

#include <vector>

namespace ve::world::mesh::detail
{
	ChunkMeshBuildResult AssembleChunkMesh(
		std::vector<MeshFace> faces,
		ChunkMeshBuildDiagnostics diagnostics);
}
