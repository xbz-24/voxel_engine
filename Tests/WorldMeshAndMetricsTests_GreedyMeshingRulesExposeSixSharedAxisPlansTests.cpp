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

TEST_CASE("greedy meshing rules expose six shared axis plans")
{
	constexpr std::array axis_plans = ve::world::mesh::GreedyMeshingRules::AxisPlans();

	CHECK(axis_plans.size() == 6U);
	CHECK(axis_plans.front().direction == ve::world::mesh::MeshFaceDirection::Top);
	CHECK(axis_plans.front().block_face == ve::blocks::BlockFace::Top);
	CHECK(axis_plans.back().direction == ve::world::mesh::MeshFaceDirection::Left);
	CHECK(axis_plans.back().normal_axis == 0);
}
namespace
{
	class RecordingRenderMesh final : public ve::rendering::RenderMesh
	{
	public:
		void Upload(const ve::rendering::MeshDescription& mesh_description) override
		{
			uploaded_vertex_count = mesh_description.vertices.size();
			uploaded_batch_count = mesh_description.batches.size();
			++upload_call_count;
		}

		void Draw() const override
		{
			++draw_call_count;
		}

		void Release() override
		{
			++release_call_count;
		}

		std::size_t uploaded_vertex_count = 0;
		std::size_t uploaded_batch_count = 0;
		int upload_call_count = 0;
		mutable int draw_call_count = 0;
		int release_call_count = 0;
	};

	class CountingRenderBackend final : public ve::rendering::RenderBackend
	{
	public:
		ve::rendering::GraphicsApi Api() const noexcept override
		{
			return ve::rendering::GraphicsApi::OpenGLCompatibility;
		}

		ve::rendering::RenderBackendCapabilities Capabilities() const noexcept override
		{
			return ve::rendering::RenderBackendCapabilities{ .is_available = true };
		}

		std::unique_ptr<ve::rendering::RenderMesh> CreateMeshResource() const override
		{
			++created_mesh_resource_count;
			return std::make_unique<RecordingRenderMesh>();
		}

		mutable int created_mesh_resource_count = 0;
	};
}

