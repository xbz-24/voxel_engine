namespace
{
	std::size_t LocalBlockIndex(int local_block_x, int local_block_y, int local_block_z)
	{
		return static_cast<std::size_t>(
			(local_block_x * ve::world::terrain::ChunkHeight + local_block_y) *
			ve::world::terrain::ChunkDepth + local_block_z);
	}

	void SetLocalBlock(
		std::vector<ve::blocks::BlockId>& chunk_blocks,
		int local_block_x,
		int local_block_y,
		int local_block_z,
		ve::blocks::BlockId block_id)
	{
		chunk_blocks[LocalBlockIndex(local_block_x, local_block_y, local_block_z)] = block_id;
	}
}

TEST_CASE("chunk mesh diagnostics count visible and culled block faces")
{
	std::vector<ve::blocks::BlockId> blocks(ve::world::mesh::ChunkBlockCount, ve::blocks::BlockId::Air);
	SetLocalBlock(blocks, 0, 0, 0, ve::blocks::BlockId::Dirt);
	SetLocalBlock(blocks, 1, 0, 0, ve::blocks::BlockId::Dirt);

	const ve::assets::AssetPaths asset_paths{};
	const ve::blocks::BlockRegistry block_registry(
		asset_paths,
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);
	const ve::world::mesh::ChunkMeshInput input{ 0, 0, blocks };
	const ve::world::mesh::NeighborMeshInputs neighbors{};

	const ve::world::mesh::ChunkMeshBuildResult mesh = ve::world::mesh::BuildChunkMesh(input, block_registry, neighbors);

	CHECK(mesh.diagnostics.candidate_block_face_count == 12U);
	CHECK(mesh.diagnostics.visible_block_face_count == 10U);
	CHECK(mesh.diagnostics.culled_block_face_count == 2U);
	CHECK(mesh.diagnostics.merged_face_count == 6U);
	CHECK(mesh.diagnostics.vertex_count == mesh.vertices.size());
	CHECK(mesh.diagnostics.batch_count == mesh.batches.size());
	CHECK(mesh.diagnostics.MergeRatio() == doctest::Approx(10.0f / 6.0f));
}

TEST_CASE("chunk meshing does not let transparent blocks hide opaque neighbor faces")
{
	std::vector<ve::blocks::BlockId> blocks(ve::world::mesh::ChunkBlockCount, ve::blocks::BlockId::Air);
	SetLocalBlock(blocks, 0, 0, 0, ve::blocks::BlockId::Dirt);
	SetLocalBlock(blocks, 1, 0, 0, ve::blocks::BlockId::OakLeaves);

	const ve::blocks::BlockRegistry block_registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);
	const ve::world::mesh::ChunkMeshInput input{ 0, 0, blocks };
	const ve::world::mesh::ChunkMeshBuildResult mesh = ve::world::mesh::BuildChunkMesh(
		input,
		block_registry,
		ve::world::mesh::NeighborMeshInputs{});

	CHECK(mesh.diagnostics.candidate_block_face_count == 12U);
	CHECK(mesh.diagnostics.visible_block_face_count == 11U);
	CHECK(mesh.diagnostics.culled_block_face_count == 1U);
}

TEST_CASE("greedy meshing rules expose six shared axis plans")
{
	constexpr std::array axis_plans = ve::world::mesh::GreedyMeshingRules::AxisPlans();

	CHECK(axis_plans.size() == 6U);
	CHECK(axis_plans.front().direction == ve::world::mesh::MeshFaceDirection::Top);
	CHECK(axis_plans.front().block_face == ve::blocks::BlockFace::Top);
	CHECK(axis_plans.back().direction == ve::world::mesh::MeshFaceDirection::Left);
	CHECK(axis_plans.back().normal_axis == 0);
}
