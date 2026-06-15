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

	/** Stores backend selection rules used during startup. */
	struct RenderBackendConfiguration
	{
		GraphicsApi preferred_api = GraphicsApi::Vulkan;
		bool allow_opengl_compatibility_fallback = false;
	};
}
