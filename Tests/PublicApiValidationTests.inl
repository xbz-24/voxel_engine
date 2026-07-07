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

TEST_CASE("public config validation rejects invalid display settings")
{
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow(voxel::Window("Invalid display")
			.UseMonitor(-1)
			.WithRefreshRate(-60));

	const std::vector<std::string> issues = invalid.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"window.monitor_index must be zero or greater") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"window.refresh_rate_hertz must be zero or greater") != issues.end());
}

TEST_CASE("public asset catalog validation rejects invalid search roots")
{
	voxel::AssetCatalog assets{};
	assets.SearchRoot("");

	const std::vector<std::string> issues = assets.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"asset search root must not be empty") != issues.end());
}

TEST_CASE("public engine config accepts runtime-bound asset search roots")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithAssets(voxel::AssetCatalog{}.SearchRoot("assets"));

	CHECK(config.IsValid());
}

TEST_CASE("public asset catalog validates source policies")
{
	voxel::AssetCatalog assets{};
	assets.Texture("empty-embedded", voxel::AssetSource::Embedded({}))
		.Model("bad-archive", voxel::AssetSource::Archive("", "crate.obj"))
		.Sound("bad-hot-reload", voxel::AssetSource::Embedded({ 1U }).EnableHotReload());

	const std::vector<std::string> issues = assets.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"texture embedded asset data must not be empty") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"model archive path must not be empty") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"sound asset hot reload requires a filesystem path") != issues.end());
}

TEST_CASE("public material library validation reports authoring range errors")
{
	voxel::Material invalid = voxel::Material::Named("invalid");
	invalid.base_color = { -0.1f, 0.5f, 1.2f, 1.0f };
	invalid.metallic = 2.0f;
	invalid.roughness = -0.1f;
	invalid.emission = -1.0f;

	voxel::MaterialLibrary materials{};
	materials.Add(invalid)
		.Add(voxel::Material::Named("duplicate"))
		.Add(voxel::Material::Named("duplicate"))
		.Add(voxel::Material{});

	const std::vector<std::string> issues = materials.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.base_color.r must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.base_color.b must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.metallic must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.roughness must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.emission must be zero or greater") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material name is duplicated: duplicate") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material name must not be empty") != issues.end());
}

TEST_CASE("public scene graph validation reports invalid hierarchy")
{
	voxel::SceneGraph graph{};
	graph.entities.push_back(voxel::Entity::Named("missing id"));
	(void)graph.AddEntity(voxel::Entity::Named("root").WithId(voxel::EntityId{ 7U }));
	(void)graph.AddEntity(voxel::Entity::Named("duplicate").WithId(voxel::EntityId{ 7U }));
	(void)graph.AddEntity(voxel::Entity::Named("self parent")
		.WithId(voxel::EntityId{ 8U })
		.ChildOf(voxel::EntityId{ 8U }));
	(void)graph.AddEntity(voxel::Entity::Named("missing parent")
		.WithId(voxel::EntityId{ 9U })
		.ChildOf(voxel::EntityId{ 100U }));

	const std::vector<std::string> issues = graph.Validate();

	CHECK(graph.FindEntity(voxel::InvalidEntityId) == nullptr);
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'missing id' must have a stable id") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'duplicate' has a duplicated id") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'self parent' cannot be parented to itself") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'missing parent' references a missing parent id") != issues.end());
}

TEST_CASE("public engine config validation reports missing asset and material references")
{
	voxel::AssetCatalog assets{};
	assets.Texture("albedo", "albedo.png");

	voxel::MaterialLibrary materials{};
	materials.Add(voxel::Material::Named("painted")
		.Texture("albedo")
		.NormalTexture("missing-normal"));

	voxel::SceneGraph scene_graph{};
	(void)scene_graph.AddEntity(voxel::Entity::Named("crate")
		.Model("missing-crate")
		.Material("missing-material"));

	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithAssets(assets)
		.WithMaterials(materials)
		.WithSceneGraph(scene_graph);

	const std::vector<std::string> issues = config.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"material 'painted' references missing texture asset: missing-normal") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'crate' references missing model asset: missing-crate") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'crate' references missing material: missing-material") != issues.end());
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
