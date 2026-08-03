TEST_CASE("public advanced api configures assets materials entities callbacks")
{
	bool update_called = false;
	bool diagnostics_called = false;
	bool log_called = false;
	int observed_pending_chunk_mesh_tasks = 0;
	int observed_pending_chunk_mesh_uploads = 0;

	voxel::AssetCatalog assets{};
	assets.SearchRoot("assets")
		.Texture("grass", voxel::AssetSource::File("assets/grass.png").EnableHotReload())
		.Texture("normal", "assets/normal.png")
		.Texture("roughness", "assets/roughness.png")
		.Texture("metallic", "assets/metallic.png")
		.Texture("occlusion", "assets/occlusion.png")
		.Texture("emissive", "assets/emissive.png")
		.Model("crate", voxel::AssetSource::Archive("assets/models.pack", "crate.obj"))
		.Sound("click", voxel::AssetSource::Embedded({ 1U, 2U, 3U }));

	voxel::MaterialLibrary materials{};
	materials.Add(voxel::Material::Named("glowing")
		.BaseColor({ -1.0f, 0.8f, 2.0f, 1.0f })
		.Texture("grass")
		.NormalTexture("normal")
		.RoughnessTexture("roughness")
		.MetallicTexture("metallic")
		.OcclusionTexture("occlusion")
		.EmissiveTexture("emissive")
		.Metallic(2.0f)
		.Roughness(-1.0f)
		.Emissive(-2.0f)
		.Transparent());

	voxel::SceneGraph graph{};
	graph.ComposeAs(voxel::SceneGraphCompositionMode::VoxelWorldAnchored);
	const voxel::EntityId crate_id = graph.AddEntity(voxel::Entity::Named("crate")
		.At({ 1.0f, 2.0f, 3.0f })
		.Model("crate")
		.Material("glowing"));
	const voxel::EntityId glow_id = graph.AddChild(crate_id, voxel::Entity::Named("crate glow")
		.At({ 1.0f, 3.0f, 3.0f })
		.Model("crate")
		.Material("glowing"));
	graph.Add(voxel::Light::Sun({ -1.0f, -2.0f, -1.0f }, 3.0f)
			.CastShadows()
			.UseIntensityUnit(voxel::LightIntensityUnit::Lux))
		.Add(voxel::Light::Spot({ 2.0f, 4.0f, 2.0f },
			{ -1.0f, -1.0f, 0.0f },
			{ 1.0f, 0.8f, 0.6f, 1.0f },
			350.0f,
			24.0f,
			12.0f,
			40.0f).UseIntensityUnit(voxel::LightIntensityUnit::Candela))
		.EnvironmentSettings(voxel::Environment{}
			.TimeOfDay(18.0f)
			.Fog({ 0.4f, 0.5f, 0.7f, 1.0f }, 0.02f));

	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithAssets(assets)
		.WithMaterials(materials)
		.WithSceneGraph(graph)
		.OnUpdate([&update_called](voxel::FrameContext& frame) {
			update_called = true;
			CHECK(frame.input.IsDown(voxel::Key::Unknown) == false);
			frame.commands.SetBlock(1, 64, 1, voxel::SeaLantern)
				.ClearBox(2, 64, 2, 3, 65, 3)
				.RequestClose();
		})
		.OnDiagnostics([&](const voxel::Diagnostics& diagnostics) {
			diagnostics_called = true;
			CHECK(diagnostics.render_distance_chunks >= 0);
			observed_pending_chunk_mesh_tasks = diagnostics.pending_chunk_mesh_tasks;
			observed_pending_chunk_mesh_uploads = diagnostics.pending_chunk_mesh_uploads;
		})
		.OnLog([&log_called](const std::string& line) {
			log_called = true;
			CHECK(!line.empty());
		});

	CHECK(config.assets.textures.size() == 6);
	REQUIRE(config.assets.search_roots.size() == 1);
	CHECK(config.assets.search_roots[0] == "assets");
	CHECK(config.assets.models.size() == 1);
	CHECK(config.assets.sounds.size() == 1);
	CHECK(config.assets.textures[0].source.hot_reload);
	CHECK(config.assets.models[0].source.storage == voxel::AssetStorage::PackagedArchive);
	CHECK(config.assets.sounds[0].source.storage == voxel::AssetStorage::EmbeddedData);
	REQUIRE(config.materials.materials.size() == 1);
