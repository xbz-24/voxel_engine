{
	voxel::VoxelRenderStyle invalid_style{};
	invalid_style.sun_direction = {};
	invalid_style.sun_color.x = -1.0f;
	invalid_style.sky_horizon_color.y = -1.0f;
	invalid_style.sky_zenith_color.z = -1.0f;
	invalid_style.sun_intensity = -0.1f;
	invalid_style.exposure = 0.0f;
	invalid_style.fog_start_distance = -1.0f;
	invalid_style.fog_end_distance = -2.0f;
	invalid_style.fog_strength = 1.1f;
	invalid_style.cloud_coverage = -0.1f;
	invalid_style.cloud_density = 1.1f;
	invalid_style.cloud_speed = -1.0f;
	invalid_style.surface_detail_strength = 2.1f;
	invalid_style.water_reflection_strength = -0.1f;
	invalid_style.shadow_strength = 2.1f;
	invalid_style.specular_strength = -0.1f;
	invalid_style.ambient_occlusion_strength = 2.1f;
	invalid_style.aerial_perspective_strength = -0.1f;
	invalid_style.subsurface_scattering_strength = 2.1f;
	invalid_style.cloud_light_absorption = -0.1f;
	invalid_style.directional_shadow_strength = 2.1f;
	invalid_style.directional_shadow_softness = -0.1f;
	invalid_style.directional_shadow_distance = 12.0f;

	const std::vector<std::string> issues = voxel::EngineConfig::Default()
		.WithVoxelRenderStyle(invalid_style)
		.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.sun_direction must be finite and non-zero") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.sun_color must contain finite non-negative values") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.exposure must be finite and greater than zero") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.fog_end_distance must be finite and greater than fog_start_distance") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.fog_strength must be finite and between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.cloud_speed must be finite and non-negative") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.surface_detail_strength must be finite and between 0 and 2") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.ambient_occlusion_strength must be finite and between 0 and 2") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.aerial_perspective_strength must be finite and between 0 and 2") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.subsurface_scattering_strength must be finite and between 0 and 2") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.cloud_light_absorption must be finite and between 0 and 2") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.directional_shadow_strength must be finite and between 0 and 2") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.directional_shadow_softness must be finite and between 0 and 2") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"voxel_render_style.directional_shadow_distance must be finite and between 32 and 512") != issues.end());
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

