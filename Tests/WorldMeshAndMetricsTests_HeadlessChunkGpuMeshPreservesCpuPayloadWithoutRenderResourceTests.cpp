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

TEST_CASE("headless chunk gpu mesh preserves cpu payload without render resource")
{
	ve::rendering::ChunkGpuMesh chunk_gpu_mesh;
	const std::vector<ve::rendering::ChunkVertex> uploaded_vertices{
		ve::rendering::ChunkVertex{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f }
	};
	std::vector<ve::rendering::ChunkMeshBatch> uploaded_batches{
		ve::rendering::ChunkMeshBatch{}
	};

	chunk_gpu_mesh.Upload(uploaded_vertices, std::move(uploaded_batches));
	chunk_gpu_mesh.Draw();

	CHECK(chunk_gpu_mesh.CpuMesh().vertices.size() == uploaded_vertices.size());
	CHECK(chunk_gpu_mesh.CpuMesh().batches.size() == 1U);
}

