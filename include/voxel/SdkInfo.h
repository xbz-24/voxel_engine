#pragma once

namespace voxel
{
	/** Semantic version of the public C++ SDK surface. */
	struct SdkVersion
	{
		int major = 0;
		int minor = 5;
		int patch = 0;
	};

	/** Feature flags describing which public SDK promises are active in this build. */
	struct SdkFeatures
	{
		bool vulkan_by_default = true;
		bool world_config_serialization = true;
		bool asset_search_roots = true;
		bool asset_catalog_runtime_loading = false;
		bool material_library_runtime_binding = false;
		bool runtime_update_callback = true;
		bool scene_graph_authoring = true;
		bool scene_graph_runtime_rendering = false;
		bool embeddable_frame_loop = true;
		bool configurable_voxel_render_style = true;
		bool animated_voxel_atmosphere = true;
		bool procedural_voxel_pbr = true;
		bool directional_shadow_mapping = true;
		bool directx12_runtime_backend = false;
	};

	[[nodiscard]] constexpr SdkVersion Version() noexcept { return {}; }
	[[nodiscard]] constexpr SdkFeatures Features() noexcept { return {}; }
}
