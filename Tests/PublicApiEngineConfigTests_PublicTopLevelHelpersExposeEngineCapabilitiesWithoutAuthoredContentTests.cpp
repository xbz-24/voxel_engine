#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public top-level helpers expose engine capabilities without authored content")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default();
	const voxel::SdkVersion version = voxel::Version();
	const voxel::SdkFeatures features = voxel::Features();

	CHECK(config.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(config.world.size_chunks == 8);
	CHECK(config.render_distance_chunks == 8);
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
