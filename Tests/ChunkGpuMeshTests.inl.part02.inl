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
