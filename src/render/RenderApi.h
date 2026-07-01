#pragma once

#include <cstdint>
#include <string_view>

namespace ve::rendering
{
	/** Identifies the graphics backend requested by high-level engine code. */
	enum class GraphicsApi
	{
		Vulkan,
		OpenGLCompatibility,
		DirectX12
	};

	/** Describes a backend feature, including why it may need a newer API surface. */
	struct RenderFeatureSupport
	{
		bool supported = false;
		std::uint32_t required_major_version = 0;
		std::uint32_t required_minor_version = 0;
		std::string_view required_extension;
	};

	/** Conservative limits exposed before a concrete device is selected. */
	struct RenderBackendLimits
	{
		std::uint32_t max_texture_dimension_2d = 0;
		std::uint32_t max_color_attachments = 0;
	};

	/** Describes backend features and baseline limits exposed to the rest of the engine. */
	struct RenderBackendCapabilities
	{
		RenderFeatureSupport compute;
		RenderFeatureSupport indirect_draw;
		RenderFeatureSupport ray_tracing;
		RenderBackendLimits limits;
		bool is_available = false;
	};

	/** Tracks how far a backend has moved onto backend-neutral engine contracts. */
	struct RenderBackendMigrationStatus
	{
		// TODO: Remove this once migration is complete and enforce backend-neutral behavior through tests instead.
		bool has_backend_neutral_meshes = false;
		bool has_backend_neutral_textures = false;
		bool has_backend_neutral_hud = false;
		bool has_backend_neutral_world_renderer = false;
		bool has_runtime_smoke_test = false;
	};

	/** Stores backend selection rules used during startup. */
	struct RenderBackendConfiguration
	{
		// TODO: Add validation/selection policy for "best available", "exact backend only", and "headless" modes.
		GraphicsApi preferred_api = GraphicsApi::Vulkan;
		bool allow_opengl_compatibility_fallback = false;
	};
}
