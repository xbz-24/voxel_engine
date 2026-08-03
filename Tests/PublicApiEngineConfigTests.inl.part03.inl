TEST_CASE("public top-level helpers expose the shortest startup path")
{
	const voxel::EngineConfig config = voxel::DesertDemo();
	const voxel::EngineConfig aqua = voxel::AquaModelDemo();
	const voxel::EngineConfig sponza = voxel::SponzaAtriumDemo();
	const voxel::EngineConfig legacy_alias = voxel::MinecraftDemo();
	const voxel::SdkVersion version = voxel::Version();
	const voxel::SdkFeatures features = voxel::Features();

	CHECK(config.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(config.world.size_chunks == 20);
	CHECK(config.render_distance_chunks == 16);
	CHECK(config.window.title == "Voxel Engine - Crystal Jungle");
	CHECK(config.demo_scene == voxel::DemoScene::HyperrealDesert);
	CHECK(aqua.demo_scene == voxel::DemoScene::AquaModel);
	CHECK(sponza.demo_scene == voxel::DemoScene::SponzaAtrium);
	CHECK(aqua.window.title == "Voxel Engine - Aqua Model Preview");
	CHECK(sponza.window.title == "Voxel Engine - Sponza Atrium Preview");
	CHECK(aqua.voxel_render_style.aerial_perspective_strength == doctest::Approx(1.22f));
	CHECK(sponza.voxel_render_style.ambient_occlusion_strength == doctest::Approx(1.34f));
	CHECK(legacy_alias.window.title == config.window.title);
	CHECK(version.major == 0);
	CHECK(version.minor == 5);
	CHECK(features.vulkan_by_default);
	CHECK(features.world_config_serialization);
	CHECK(features.asset_search_roots);
	CHECK(!features.asset_catalog_runtime_loading);
	CHECK(!features.material_library_runtime_binding);
	CHECK(features.runtime_update_callback);
	CHECK(features.scene_graph_authoring);
	CHECK(!features.scene_graph_runtime_rendering);
	CHECK(features.embeddable_frame_loop);
	CHECK(features.configurable_voxel_render_style);
	CHECK(features.animated_voxel_atmosphere);
	CHECK(features.procedural_voxel_pbr);
	CHECK(features.directional_shadow_mapping);
	CHECK(!features.directx12_runtime_backend);
}
