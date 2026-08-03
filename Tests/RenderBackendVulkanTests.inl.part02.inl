	const ve::rendering::VulkanVoxelFrameUniforms packed =
		ve::rendering::PackVulkanVoxelFrameUniforms(
			style,
			{ 4.0f, 5.0f, 6.0f },
			{ 0.0f, 0.0f, -1.0f },
			12.5f,
			VkExtent2D{ 1920U, 1080U });

	CHECK(sizeof(ve::rendering::VulkanVoxelTransformPushConstants) == 64U);
	CHECK(sizeof(ve::rendering::VulkanVoxelFrameUniforms) == 224U);
	CHECK(offsetof(ve::rendering::VulkanVoxelFrameUniforms, atmosphere_parameters) == 96U);
	CHECK(offsetof(ve::rendering::VulkanVoxelFrameUniforms, surface_parameters) == 112U);
	CHECK(offsetof(ve::rendering::VulkanVoxelFrameUniforms, advanced_lighting_parameters) == 128U);
	CHECK(offsetof(ve::rendering::VulkanVoxelFrameUniforms, directional_shadow_parameters) == 144U);
	CHECK(offsetof(ve::rendering::VulkanVoxelFrameUniforms, light_view_projection) == 160U);
	CHECK(packed.camera_position_and_time.x == doctest::Approx(4.0f));
	CHECK(packed.camera_position_and_time.w == doctest::Approx(12.5f));
	CHECK(packed.sun_direction_and_intensity.x == doctest::Approx(1.0f));
	CHECK(packed.sun_direction_and_intensity.w == doctest::Approx(1.4f));
	CHECK(packed.sun_color_and_exposure.w == doctest::Approx(1.2f));
	CHECK(packed.sky_horizon_color_and_fog_start.w == doctest::Approx(90.0f));
	CHECK(packed.sky_zenith_color_and_fog_end.w == doctest::Approx(330.0f));
	CHECK(packed.viewport_size_and_inverse.x == doctest::Approx(1920.0f));
	CHECK(packed.viewport_size_and_inverse.w == doctest::Approx(1.0f / 1080.0f));
	CHECK(packed.atmosphere_parameters.y == doctest::Approx(0.35f));
	CHECK(packed.atmosphere_parameters.z == doctest::Approx(0.72f));
	CHECK(packed.surface_parameters.x == doctest::Approx(1.3f));
	CHECK(packed.surface_parameters.y == doctest::Approx(1.5f));
	CHECK(packed.advanced_lighting_parameters.x == doctest::Approx(1.25f));
	CHECK(packed.advanced_lighting_parameters.y == doctest::Approx(0.85f));
	CHECK(packed.advanced_lighting_parameters.z == doctest::Approx(1.45f));
	CHECK(packed.advanced_lighting_parameters.w == doctest::Approx(0.75f));
	CHECK(packed.directional_shadow_parameters.x == doctest::Approx(1.1f));
	CHECK(packed.directional_shadow_parameters.y == doctest::Approx(0.65f));
	CHECK(packed.directional_shadow_parameters.z == doctest::Approx(192.0f));
	CHECK(packed.directional_shadow_parameters.w == doctest::Approx(1.0f / 2048.0f));
	CHECK(packed.light_view_projection != glm::mat4{ 1.0f });
	const glm::mat4 stable_shadow_matrix = ve::rendering::BuildVulkanDirectionalShadowMatrix(
		{ 4.0f, 5.0f, 6.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ -0.42f, 0.78f, -0.46f },
		192.0f);
	const glm::mat4 sub_texel_movement_matrix = ve::rendering::BuildVulkanDirectionalShadowMatrix(
		{ 4.01f, 5.0f, 6.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ -0.42f, 0.78f, -0.46f },
		192.0f);
	CHECK(sub_texel_movement_matrix[3][0] == doctest::Approx(stable_shadow_matrix[3][0]).epsilon(0.00001));
	CHECK(sub_texel_movement_matrix[3][1] == doctest::Approx(stable_shadow_matrix[3][1]).epsilon(0.00001));
}

TEST_CASE("vulkan demo profiles centralize startup and scene defaults")
{
	const ve::rendering::VulkanMinecraftDemoProfile crystal =
		ve::rendering::VulkanMinecraftDemoProfileFor(ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert);
	const ve::rendering::VulkanMinecraftDemoProfile aqua =
		ve::rendering::VulkanMinecraftDemoProfileFor(ve::rendering::VulkanMinecraftDemoPreset::AquaModel);
	const ve::rendering::VulkanMinecraftDemoProfile sponza =
		ve::rendering::VulkanMinecraftDemoProfileFor(ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium);

	CHECK(std::string{ crystal.window_title } == "Voxel Engine - Crystal Jungle");
	CHECK(crystal.scene.preset == ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert);
	CHECK(crystal.model_asset_keyword.empty());
	CHECK(aqua.world_size_chunks == 16);
	CHECK(aqua.render_distance_chunks == 12);
	CHECK(aqua.scene.preset == ve::rendering::VulkanMinecraftDemoPreset::AquaModel);
	CHECK(std::string{ aqua.model_asset_keyword } == "aqua");
	CHECK(aqua.model_voxel_budget > 0u);
	CHECK(sponza.world_size_chunks == 20);
	CHECK(sponza.scene.preset == ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium);
	CHECK(std::string{ sponza.model_asset_keyword } == "sponza");
	CHECK(sponza.model_target_extent > aqua.model_target_extent);
}

TEST_CASE("vulkan software rasterizer data rejects malformed samples")
{
	ve::rendering::VulkanRasterFrameWorldSnapshot snapshot;
	snapshot.width = 1;
	snapshot.height = 1;
	snapshot.depth = 1;

	CHECK(snapshot.GetBlock({ 0, 0, 0 }) == ve::blocks::BlockId::Air);
	snapshot.blocks = { ve::blocks::BlockId::Stone };
	CHECK(snapshot.GetBlock({ 0, 0, 0 }) == ve::blocks::BlockId::Stone);
	CHECK(snapshot.GetBlock({ 1, 0, 0 }) == ve::blocks::BlockId::Air);

	ve::rendering::VulkanRasterCpuTexture malformed_texture;
	malformed_texture.pixels = { 0x00abcdefU };
