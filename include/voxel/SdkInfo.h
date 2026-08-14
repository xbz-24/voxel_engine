#pragma once

/** @defgroup voxel_sdk Public C++ SDK
 * Stable configuration, authoring, and runtime-hosting contracts.
 */
/** @defgroup voxel_sdk_startup Engine startup and hosting
 * @ingroup voxel_sdk
 * Configure validated runtime layouts, launch, or externally drive an engine instance.
 */
/** @defgroup voxel_sdk_world_editing World editing
 * @ingroup voxel_sdk
 * Build deterministic block edits and queue them at frame boundaries.
 */
/** @defgroup voxel_sdk_runtime_callbacks Runtime callbacks
 * @ingroup voxel_sdk
 * Observe frame input and diagnostics, then return deferred commands.
 */
/** @defgroup voxel_sdk_assets Asset authoring
 * @ingroup voxel_sdk
 * Describe and validate asset sources; runtime loading is capability-gated.
 */
/** @defgroup voxel_sdk_materials Material authoring
 * @ingroup voxel_sdk
 * Describe and validate materials; runtime binding is capability-gated.
 */
/** @defgroup voxel_sdk_scene_graph Scene graph authoring
 * @ingroup voxel_sdk
 * Compose entities, lights, and environments; runtime rendering is capability-gated.
 */

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
