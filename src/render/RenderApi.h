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
		// Temporary migration metadata until backend-neutral behavior has complete test coverage.
		bool has_backend_neutral_meshes = false;
		bool has_backend_neutral_textures = false;
		bool has_backend_neutral_hud = false;
		bool has_backend_neutral_world_renderer = false;
		bool has_runtime_smoke_test = false;
	};

	enum class RenderBackendSelectionPolicy
	{
		ExactBackendOnly,
		BestAvailable,
		Headless
	};

	/** Stores backend selection rules used during startup. */
	struct RenderBackendConfiguration
	{
		GraphicsApi preferred_api = GraphicsApi::Vulkan;
		RenderBackendSelectionPolicy selection_policy = RenderBackendSelectionPolicy::ExactBackendOnly;
		bool allow_opengl_compatibility_fallback = false;
	};

	struct RenderBackendSelection
	{
		GraphicsApi api = GraphicsApi::Vulkan;
		bool headless = false;
	};
}
