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
	CHECK(config.materials.materials[0].transparent);
	CHECK(config.materials.materials[0].base_color.r == doctest::Approx(0.0f));
	CHECK(config.materials.materials[0].base_color.b == doctest::Approx(1.0f));
	CHECK(config.materials.materials[0].metallic == doctest::Approx(1.0f));
	CHECK(config.materials.materials[0].roughness == doctest::Approx(0.0f));
	CHECK(config.materials.materials[0].emission == doctest::Approx(0.0f));
	CHECK(config.materials.materials[0].normal_texture == "normal");
	CHECK(config.materials.Validate().empty());
	REQUIRE(config.scene_graph.entities.size() == 2);
	CHECK(crate_id.IsValid());
	CHECK(glow_id.IsValid());
	CHECK(config.scene_graph.entities[0].id == crate_id);
	CHECK(config.scene_graph.entities[0].transform.position.z == doctest::Approx(3.0f));
	CHECK(config.scene_graph.entities[1].parent == crate_id);
	REQUIRE(config.scene_graph.FindEntity(glow_id) != nullptr);
	CHECK(config.scene_graph.FindEntity(glow_id)->name == "crate glow");
	CHECK(config.scene_graph.Validate().empty());
	REQUIRE(config.scene_graph.lights.size() == 2);
	CHECK(config.scene_graph.lights[0].shadows.enabled);
	CHECK(config.scene_graph.lights[0].intensity_unit == voxel::LightIntensityUnit::Lux);
	CHECK(config.scene_graph.lights[1].kind == voxel::LightKind::Spot);
	CHECK(config.scene_graph.lights[1].inner_cone_degrees == doctest::Approx(12.0f));
	CHECK(config.scene_graph.lights[1].outer_cone_degrees == doctest::Approx(40.0f));
	CHECK(config.scene_graph.lights[1].intensity_unit == voxel::LightIntensityUnit::Candela);
	CHECK(config.scene_graph.environment.time_of_day == doctest::Approx(18.0f));

	voxel::FrameContext frame{};
	frame.input.f1 = true;
	frame.input.primary_action = true;
	frame.camera.position = { 1.0f, 2.0f, 3.0f };
	frame.camera.forward = { 0.0f, 0.0f, -1.0f };
	frame.selected_block = voxel::Cobblestone;
	frame.hit_result = voxel::BlockHitResult{
		true,
		voxel::At(1, 64, 1),
		voxel::At(1, 65, 1),
		voxel::Stone
	};
	config.on_update(frame);
	CHECK(update_called);
	CHECK(frame.input.IsDown(voxel::Key::F1));
	CHECK(frame.input.IsActive(voxel::InputAction::ToggleDebugOverlay));
	CHECK(frame.input.IsActive(voxel::InputAction::PrimaryAction));
	CHECK(frame.camera.position.y == doctest::Approx(2.0f));
	CHECK(frame.camera.forward.z == doctest::Approx(-1.0f));
	CHECK(frame.selected_block == voxel::Cobblestone);
	CHECK(frame.hit_result.has_hit);
	CHECK(frame.hit_result.target_block.y == 64);
	CHECK(frame.hit_result.target_block_type == voxel::Stone);
	CHECK(frame.commands.world_edits.size() == 2);
	CHECK(frame.commands.request_close);

	voxel::RuntimeCommands typed_commands{};
	typed_commands.FillBox(voxel::At(5, 64, 5), voxel::At(4, 63, 4), voxel::Stone)
		.ClearBox(voxel::At(5, 65, 5), voxel::At(4, 65, 4));
	REQUIRE(typed_commands.world_edits.size() == 2);
	CHECK(typed_commands.world_edits[0].box.minimum.x == 4);
	CHECK(typed_commands.world_edits[0].box.minimum.y == 63);
	CHECK(typed_commands.world_edits[0].box.maximum.z == 5);
	CHECK(typed_commands.world_edits[1].block == voxel::Air);

	config.on_diagnostics(voxel::Diagnostics{ 60.0, 2, 4, 5, 6, 7 });
	CHECK(diagnostics_called);
	CHECK(observed_pending_chunk_mesh_tasks == 5);
	CHECK(observed_pending_chunk_mesh_uploads == 6);

	config.on_log("public api log");
	CHECK(log_called);

	const std::vector<std::string> validation_issues = config.Validate();
	CHECK(std::find(validation_issues.begin(), validation_issues.end(),
		"AssetCatalog runtime loading is not implemented") != validation_issues.end());
	CHECK(std::find(validation_issues.begin(), validation_issues.end(),
		"MaterialLibrary runtime binding is not implemented") != validation_issues.end());
	CHECK(std::find(validation_issues.begin(), validation_issues.end(),
		"SceneGraph runtime rendering is not implemented") != validation_issues.end());
}
