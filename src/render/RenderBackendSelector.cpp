#include "RenderBackendSelector.h"

namespace ve::rendering
{
	/** Returns the explicitly configured backend; Vulkan is the default configuration. */
	GraphicsApi RenderBackendSelector::SelectApi(const RenderBackendConfiguration& configuration) noexcept
	{
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
