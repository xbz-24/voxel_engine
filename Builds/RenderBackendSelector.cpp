#include "RenderBackendSelector.h"

namespace ve::rendering
{
	/** Selects Vulkan unless the current renderer still requires legacy OpenGL calls. */
	GraphicsApi RenderBackendSelector::SelectApi(const RenderBackendConfiguration& configuration, bool requires_legacy_opengl) noexcept
	{
		if (requires_legacy_opengl && configuration.allow_opengl_compatibility_fallback) return GraphicsApi::OpenGLCompatibility;
		return configuration.preferred_api;
	}

	/** Returns a stable debug name for a graphics API. */
	const char* RenderBackendSelector::Name(GraphicsApi api) noexcept
	{
		switch (api)
		{
		case GraphicsApi::Vulkan: return "Vulkan";
		case GraphicsApi::OpenGLCompatibility: return "OpenGLCompatibility";
		case GraphicsApi::DirectX12: return "DirectX12";
		}
		return "Unknown";
	}
}
