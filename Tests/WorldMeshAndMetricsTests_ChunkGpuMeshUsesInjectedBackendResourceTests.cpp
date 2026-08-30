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
#include "WorldMeshTestSupport.h"

#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>

TEST_CASE("chunk gpu mesh uses injected backend resource")
{
	auto render_mesh_resource = std::make_unique<RecordingRenderMesh>();
	RecordingRenderMesh* recording_render_mesh = render_mesh_resource.get();
	ve::rendering::ChunkGpuMesh chunk_gpu_mesh(std::move(render_mesh_resource));
	const std::vector<ve::rendering::ChunkVertex> uploaded_vertices{
		ve::rendering::ChunkVertex{ 0.0f, 1.0f, 2.0f, 0.0f, 1.0f, 1.0f, 0.5f, 0.25f }
	};
	std::vector<ve::rendering::ChunkMeshBatch> uploaded_batches{
		ve::rendering::ChunkMeshBatch{}
	};

	chunk_gpu_mesh.Upload(uploaded_vertices, std::move(uploaded_batches));
	chunk_gpu_mesh.Draw();

	CHECK(recording_render_mesh->upload_call_count == 1);
	CHECK(recording_render_mesh->uploaded_vertex_count == uploaded_vertices.size());
	CHECK(recording_render_mesh->uploaded_batch_count == 1U);
	CHECK(recording_render_mesh->draw_call_count == 1);
	CHECK(chunk_gpu_mesh.CpuMesh().vertices.size() == uploaded_vertices.size());
	CHECK(chunk_gpu_mesh.CpuMesh().batches.size() == 1U);

	chunk_gpu_mesh.Release();

	CHECK(recording_render_mesh->release_call_count == 1);
	CHECK(chunk_gpu_mesh.CpuMesh().vertices.empty());
	CHECK(chunk_gpu_mesh.CpuMesh().batches.empty());
}

