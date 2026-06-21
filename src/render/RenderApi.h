#pragma once

namespace ve::rendering
{
	/** Identifies the graphics backend requested by high-level engine code. */
	enum class GraphicsApi
	{
		Vulkan,
		OpenGLCompatibility,
		DirectX12
	};

	/** Describes broad backend features exposed to the rest of the engine. */
	struct RenderBackendCapabilities
	{
		bool supports_compute = false;
		bool supports_indirect_draw = false;
		bool supports_ray_tracing = false;
		bool is_available = false;
	};

	/** Tracks how far a backend has moved onto backend-neutral engine contracts. */
	struct RenderBackendMigrationStatus
	{
		bool has_backend_neutral_meshes = false;
		bool has_backend_neutral_textures = false;
		bool has_backend_neutral_hud = false;
		bool has_backend_neutral_world_renderer = false;
		bool has_runtime_smoke_test = false;
	};

	/** Stores backend selection rules used during startup. */
	struct RenderBackendConfiguration
	{
		GraphicsApi preferred_api = GraphicsApi::Vulkan;
		bool allow_opengl_compatibility_fallback = false;
	};
}
