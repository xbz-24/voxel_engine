#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public engine config separates startup and runtime tuning views")
{
	const voxel::VoxelRenderStyle render_style = voxel::VoxelRenderStyle{}
		.WithSunDirection({ -0.25f, 0.90f, -0.35f })
		.WithSunColor({ 1.40f, 1.10f, 0.82f })
		.WithSunIntensity(1.25f)
		.WithSkyColors({ 0.60f, 0.72f, 0.80f }, { 0.24f, 0.46f, 0.78f })
		.WithExposure(1.10f)
		.WithFogRange(90.0f, 360.0f)
		.WithFogStrength(0.58f)
		.WithClouds(0.36f, 0.68f, 0.024f)
		.WithSurfaceDetail(1.2f)
		.WithWaterReflections(1.4f)
		.WithShadowStrength(0.9f)
		.WithSpecularStrength(1.3f)
		.WithAmbientOcclusion(1.1f)
		.WithAerialPerspective(0.8f)
		.WithSubsurfaceScattering(1.4f)
		.WithCloudLightAbsorption(0.7f)
		.WithDirectionalShadows(1.15f, 0.75f, 196.0f);
	const voxel::EngineRuntimeTuning tuning{
		4,
		false,
		false
	};
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithWindow("Split Config", 960, 540)
		.WithVoxelRenderStyle(render_style)
		.WithRuntimeTuning(tuning);

	const voxel::EngineStartupConfig startup = config.StartupConfig();
	const voxel::EngineRuntimeTuning runtime = config.RuntimeTuning();

	CHECK(startup.window.title == "Split Config");
	CHECK(startup.window.width == 960);
	CHECK(startup.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(startup.voxel_render_style.sun_intensity == doctest::Approx(1.25f));
	CHECK(startup.voxel_render_style.sky_zenith_color.z == doctest::Approx(0.78f));
	CHECK(startup.voxel_render_style.fog_start_distance == doctest::Approx(90.0f));
	CHECK(startup.voxel_render_style.fog_strength == doctest::Approx(0.58f));
	CHECK(startup.voxel_render_style.cloud_coverage == doctest::Approx(0.36f));
	CHECK(startup.voxel_render_style.cloud_density == doctest::Approx(0.68f));
	CHECK(startup.voxel_render_style.water_reflection_strength == doctest::Approx(1.4f));
	CHECK(startup.voxel_render_style.specular_strength == doctest::Approx(1.3f));
	CHECK(startup.voxel_render_style.ambient_occlusion_strength == doctest::Approx(1.1f));
	CHECK(startup.voxel_render_style.aerial_perspective_strength == doctest::Approx(0.8f));
	CHECK(startup.voxel_render_style.subsurface_scattering_strength == doctest::Approx(1.4f));
	CHECK(startup.voxel_render_style.cloud_light_absorption == doctest::Approx(0.7f));
	CHECK(startup.voxel_render_style.directional_shadow_strength == doctest::Approx(1.15f));
	CHECK(startup.voxel_render_style.directional_shadow_softness == doctest::Approx(0.75f));
	CHECK(startup.voxel_render_style.directional_shadow_distance == doctest::Approx(196.0f));
	CHECK(runtime.render_distance_chunks == 4);
	CHECK(!runtime.show_debug_overlay);
	CHECK(!runtime.enable_settings_menu);
}

