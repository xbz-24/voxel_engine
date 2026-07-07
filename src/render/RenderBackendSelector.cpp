#include "RenderBackendSelector.h"

#include "RenderBackendCatalog.h"

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] GraphicsApi FirstImplementedBackend() noexcept
		{
			for (const RenderBackendDescriptor& backend : RenderBackendCatalog::Backends())
			{
				if (backend.is_default && backend.is_implemented) return backend.api;
			}
			for (const RenderBackendDescriptor& backend : RenderBackendCatalog::Backends())
			{
				if (backend.is_implemented) return backend.api;
			}
			return RenderBackendCatalog::DefaultBackend().api;
		}
	}

	/** Applies the configured selection policy. */
	RenderBackendSelection RenderBackendSelector::Select(const RenderBackendConfiguration& configuration) noexcept
	{
		if (configuration.selection_policy == RenderBackendSelectionPolicy::Headless)
		{
			return RenderBackendSelection{ configuration.preferred_api, true };
		}

		const RenderBackendDescriptor& preferred_backend = RenderBackendCatalog::Find(configuration.preferred_api);
		if (configuration.selection_policy == RenderBackendSelectionPolicy::BestAvailable && !preferred_backend.is_implemented)
		{
			return RenderBackendSelection{ FirstImplementedBackend(), false };
		}

		if (configuration.allow_opengl_compatibility_fallback && !preferred_backend.is_implemented)
		{
			const RenderBackendDescriptor& opengl_backend = RenderBackendCatalog::Find(GraphicsApi::OpenGLCompatibility);
			if (opengl_backend.is_implemented) return RenderBackendSelection{ GraphicsApi::OpenGLCompatibility, false };
		}

		return RenderBackendSelection{ configuration.preferred_api, false };
	}

	/** Returns the selected graphics API; Vulkan is the default configuration. */
	GraphicsApi RenderBackendSelector::SelectApi(const RenderBackendConfiguration& configuration) noexcept
	{
		return Select(configuration).api;
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
