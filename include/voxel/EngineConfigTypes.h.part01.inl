
#include "voxel/Camera.h"

#include <string>

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

	/** Returns the SDK version compiled into this header. */
	[[nodiscard]] constexpr SdkVersion Version() noexcept
	{
		return {};
	}

	/** Returns the feature flags compiled into this header. */
	[[nodiscard]] constexpr SdkFeatures Features() noexcept
	{
		return {};
	}

	/** Requested graphics backend for EngineConfig startup. */
	enum class GraphicsApi
	{
		Vulkan,
		OpenGLCompatibility,
		DirectX12
	};

	/** Backend-neutral lighting, sky, tone-mapping, and fog settings for voxel rendering. */
	struct VoxelRenderStyle
	{
		Vec3 sun_direction{ -0.42f, 0.78f, -0.46f };
		Vec3 sun_color{ 1.16f, 1.04f, 0.84f };
		Vec3 sky_horizon_color{ 0.72f, 0.70f, 0.64f };
		Vec3 sky_zenith_color{ 0.52f, 0.68f, 0.88f };
		float sun_intensity = 1.0f;
		float exposure = 1.0f;
		float fog_start_distance = 165.0f;
		float fog_end_distance = 455.0f;
		float fog_strength = 0.42f;
		float cloud_coverage = 0.42f;
		float cloud_density = 0.62f;
		float cloud_speed = 0.018f;
		float surface_detail_strength = 1.0f;
		float water_reflection_strength = 1.0f;
		float shadow_strength = 1.0f;
		float specular_strength = 1.0f;
		float ambient_occlusion_strength = 1.0f;
		float aerial_perspective_strength = 1.0f;
		float subsurface_scattering_strength = 1.0f;
		float cloud_light_absorption = 1.0f;
		float directional_shadow_strength = 0.85f;
		float directional_shadow_softness = 1.0f;
		float directional_shadow_distance = 220.0f;
