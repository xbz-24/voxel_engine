#include "RenderBackendCatalog.h"

namespace ve::rendering
{
	namespace
	{
		constexpr std::array kBackends{
			RenderBackendDescriptor{ GraphicsApi::Vulkan, "Vulkan", true, true },
			RenderBackendDescriptor{ GraphicsApi::OpenGLCompatibility, "OpenGLCompatibility", false, true },
			RenderBackendDescriptor{ GraphicsApi::DirectX12, "DirectX12", false, false }
		};
	}

	/** Returns every graphics backend known by the engine. */
	std::span<const RenderBackendDescriptor> RenderBackendCatalog::Backends() noexcept
	{
		return kBackends;
	}

	/** Returns the default backend descriptor. */
	const RenderBackendDescriptor& RenderBackendCatalog::DefaultBackend() noexcept
	{
		return kBackends.front();
	}

	/** Finds a backend descriptor by API. */
	const RenderBackendDescriptor& RenderBackendCatalog::Find(GraphicsApi api) noexcept
	{
		for (const RenderBackendDescriptor& backend : kBackends)
		{
			if (backend.api == api) return backend;
		}
		return DefaultBackend();
	}
}
