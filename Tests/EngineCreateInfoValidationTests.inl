TEST_CASE("engine create info accepts the default internal configuration")
{
	std::optional<ve::engine::ValidatedEngineCreateInfo> validated =
		ve::engine::MakeValidatedEngineCreateInfo(ve::engine::EngineCreateInfo{});

	REQUIRE(validated.has_value());
	CHECK(validated->Value().window.width == 1280);
	CHECK(validated->Value().world_size_chunks == 8);
}

TEST_CASE("engine create info validation rejects unchecked runtime configuration")
{
	ve::engine::EngineCreateInfo invalid{};
	invalid.window.width = 0;
	invalid.window.height = -1;
	invalid.window.display_index = -1;
	invalid.window.refresh_rate_hertz = -60;
	invalid.render_backend.preferred_api = ve::tests::InvalidEnumValue<ve::rendering::GraphicsApi>();
	invalid.render_backend.selection_policy = ve::tests::InvalidEnumValue<ve::rendering::RenderBackendSelectionPolicy>();
	invalid.vulkan_demo_preset = ve::tests::InvalidEnumValue<ve::rendering::VulkanMinecraftDemoPreset>();
	invalid.voxel_render_style.sun_direction = {};
	invalid.voxel_render_style.sun_color.x = -1.0f;
	invalid.voxel_render_style.sun_intensity = -1.0f;
	invalid.voxel_render_style.exposure = 0.0f;
	invalid.voxel_render_style.fog_start_distance = -1.0f;
	invalid.voxel_render_style.fog_end_distance = -2.0f;
	invalid.world_size_chunks = 0;
	invalid.render_distance_chunks = -1;
	invalid.has_custom_camera = true;
	invalid.camera_position.x = std::numeric_limits<float>::infinity();
	invalid.camera_look_at.y = std::numeric_limits<float>::quiet_NaN();
	invalid.world_edits.push_back(ve::world::MakeSingleBlockEdit(1, 2, 3, ve::blocks::BlockId::Stone));
	invalid.world_edits.back().kind = ve::tests::InvalidEnumValue<ve::world::WorldBlockEdit::Kind>();

	std::vector<std::string> builder_issues;
	const std::vector<std::string> issues = ve::engine::ValidateEngineCreateInfo(invalid);
	const std::optional<ve::engine::ValidatedEngineCreateInfo> validated =
		ve::engine::MakeValidatedEngineCreateInfo(invalid, &builder_issues);
	const auto contains_issue = [&issues](std::string_view expected_issue) {
		return std::find(issues.begin(), issues.end(), expected_issue) != issues.end();
	};

	CHECK(!validated.has_value());
	CHECK(builder_issues == issues);
	CHECK(contains_issue("window.width must be greater than zero"));
	CHECK(contains_issue("window.height must be greater than zero"));
	CHECK(contains_issue("window.display_index must be zero or greater"));
	CHECK(contains_issue("window.refresh_rate_hertz must be zero or greater"));
	CHECK(contains_issue("render_backend.preferred_api is not a known graphics api"));
	CHECK(contains_issue("render_backend.selection_policy is not a known selection policy"));
	CHECK(contains_issue("vulkan_demo_preset is not a known demo preset"));
	CHECK(contains_issue("voxel_render_style.sun_direction must be finite and non-zero"));
	CHECK(contains_issue("voxel_render_style colors must contain finite non-negative values"));
	CHECK(contains_issue("voxel_render_style.sun_intensity must be finite and non-negative"));
	CHECK(contains_issue("voxel_render_style.exposure must be finite and greater than zero"));
	CHECK(contains_issue("voxel_render_style.fog_start_distance must be finite and non-negative"));
	CHECK(contains_issue("voxel_render_style.fog_end_distance must be finite and greater than fog_start_distance"));
	CHECK(contains_issue("world_size_chunks must be greater than zero"));
	CHECK(contains_issue("render_distance_chunks must be zero or greater"));
	CHECK(contains_issue("camera_position must contain finite values"));
	CHECK(contains_issue("camera_look_at must contain finite values"));
	CHECK(contains_issue("world_edits contains an unknown edit kind"));
}

TEST_CASE("engine application rejects invalid direct create info before startup")
{
	ve::engine::EngineCreateInfo invalid{};
	invalid.window.width = 0;

	EngineApplication engine{ invalid };

	CHECK(engine.Run() == -1);
}
