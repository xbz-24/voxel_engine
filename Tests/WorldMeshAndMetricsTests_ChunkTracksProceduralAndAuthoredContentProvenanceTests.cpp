#include <doctest/doctest.h>

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "ChunkGpuMesh.h"
#include "ChunkMeshBuilder.h"
#include "ChunkMeshInput.h"
#include "ComputeTerrainGenerator.h"
#include "GreedyChunkMesher.h"
#include "RenderBackend.h"
#include "World.h"
#include "WorldBlockEdit.h"
#include "WorldCoordinates.h"
#include "TestTypeHelpers.h"

#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>

TEST_CASE("chunk tracks procedural and authored content provenance")
{
	Chunk empty_chunk(0, 0, ChunkGenerationMode::Empty);

	CHECK(empty_chunk.Provenance() == ChunkContentProvenance::Empty);
	CHECK(!empty_chunk.HasProceduralTerrain());
	CHECK(!empty_chunk.HasAuthoredEdits());

	empty_chunk.Generate();
	CHECK(empty_chunk.Provenance() == ChunkContentProvenance::ProceduralTerrain);
	CHECK(empty_chunk.HasProceduralTerrain());
	CHECK(!empty_chunk.HasAuthoredEdits());

	CHECK(empty_chunk.SetBlock(0, 1, 0, ve::blocks::BlockId::DiamondOre));
	CHECK(empty_chunk.Provenance() == ChunkContentProvenance::ProceduralTerrainWithAuthoredEdits);
	CHECK(empty_chunk.HasAuthoredEdits());
}
