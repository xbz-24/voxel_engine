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

TEST_CASE("world creates chunk mesh resources through active render backend")
{
	CountingRenderBackend render_backend;
	ve::world::World world(ve::world::CreateInfoForSquareWorld(2));
	world.SetRenderBackend(&render_backend);

	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 2 });
	CHECK(render_backend.created_mesh_resource_count == 4);

	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	CHECK(render_backend.created_mesh_resource_count == 5);
}
