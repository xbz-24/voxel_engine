TEST_CASE("public config validation reports invalid startup values")
{
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow("Invalid", 0, -1)
		.WithWorldSizeChunks(0)
		.WithRenderDistanceChunks(-1)
		.UseDirectX12()
		.WithAssets(voxel::AssetCatalog{}
			.Texture("duplicate", "a.png")
			.Texture("duplicate", "b.png")
			.Model("", ""));

	const std::vector<std::string> issues = invalid.Validate();

	CHECK(!invalid.IsValid());
	CHECK(issues.size() == 9);
	CHECK(std::find(issues.begin(), issues.end(),
		"AssetCatalog runtime loading is not implemented") != issues.end());
}

TEST_CASE("public engine rejects invalid config before runtime startup")
{
	std::vector<std::string> logs;
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow("Invalid", 0, 720)
		.OnLog([&logs](const std::string& line) {
			logs.push_back(line);
		});

	voxel::Engine engine{ invalid };

	CHECK(engine.Run() == -1);
	REQUIRE(!logs.empty());
	CHECK(logs.front() == "Invalid EngineConfig: window.width must be greater than zero");
}

TEST_CASE("public world serialization roundtrips config edits")
{
	const std::filesystem::path path = std::filesystem::temp_directory_path() / "voxel_public_world_roundtrip.voxelscene";
	const voxel::WorldConfig original = voxel::World(5)
		.SetBlock(1, 2, 3, voxel::DiamondOre)
		.FillBox(-1, 0, -1, 1, 0, 1, voxel::Grass);

	REQUIRE(voxel::SaveWorldConfig(original, path.string()));
	const voxel::WorldConfig loaded = voxel::LoadWorldConfig(path.string());
	std::filesystem::remove(path);

	CHECK(loaded.size_chunks == 5);
	REQUIRE(loaded.edits.size() == 2);
	CHECK(loaded.edits[0].kind == voxel::WorldEdit::Kind::SetBlock);
	CHECK(loaded.edits[0].position.x == 1);
	CHECK(loaded.edits[0].block == voxel::DiamondOre);
	CHECK(loaded.edits[1].kind == voxel::WorldEdit::Kind::FillBox);
	CHECK(loaded.edits[1].box.minimum.x == -1);
	CHECK(loaded.edits[1].block == voxel::Grass);
}
