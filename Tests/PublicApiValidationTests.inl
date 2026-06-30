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
	const auto uniquePathSuffix = std::chrono::steady_clock::now().time_since_epoch().count();
	const std::filesystem::path path = std::filesystem::temp_directory_path() /
		("voxel_public_world_roundtrip_" + std::to_string(uniquePathSuffix) + ".voxelscene");
	const voxel::WorldConfig original = voxel::World(5)
		.WithTerrainSeed(77U)
		.WithBaseSurfaceHeight(23)
		.WithTerrainGenerator(voxel::TerrainGenerator::Flat)
		.WithTerrainBiome(voxel::TerrainBiome::Alpine)
		.WithTerrainPalette(voxel::TerrainPalette{
			voxel::Obsidian,
			voxel::Blackstone,
			voxel::Stone,
			voxel::Snow,
			voxel::Dirt,
			voxel::Snow,
			voxel::Gravel,
			voxel::Andesite
		})
		.SetBlock(1, 2, 3, voxel::DiamondOre)
		.FillBox(-1, 0, -1, 1, 0, 1, voxel::Grass);

	REQUIRE(voxel::SaveWorldConfig(original, path.string()));
	const voxel::WorldConfig loaded = voxel::LoadWorldConfig(path.string());
	std::error_code cleanupError;
	std::filesystem::remove(path, cleanupError);

	CHECK(loaded.size_chunks == 5);
	CHECK(loaded.terrain_seed == 77U);
	CHECK(loaded.base_surface_height == 23);
	CHECK(loaded.terrain_generator == voxel::TerrainGenerator::Flat);
	CHECK(loaded.terrain_biome == voxel::TerrainBiome::Alpine);
	CHECK(loaded.terrain_palette.deep_stone == voxel::Blackstone);
	CHECK(loaded.terrain_palette.gravel_patch == voxel::Andesite);
	REQUIRE(loaded.edits.size() == 2);
	CHECK(loaded.edits[0].kind == voxel::WorldEdit::Kind::SetBlock);
	CHECK(loaded.edits[0].position.x == 1);
	CHECK(loaded.edits[0].block == voxel::DiamondOre);
	CHECK(loaded.edits[1].kind == voxel::WorldEdit::Kind::FillBox);
	CHECK(loaded.edits[1].box.minimum.x == -1);
	CHECK(loaded.edits[1].block == voxel::Grass);
}
