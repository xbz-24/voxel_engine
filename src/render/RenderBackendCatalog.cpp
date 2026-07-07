#include "RenderBackendCatalog.h"

namespace ve::rendering
{
	namespace
	{
		constexpr std::array kBackends{
			RenderBackendDescriptor{ GraphicsApi::Vulkan, "Vulkan", true, true, RenderBackendMigrationStatus{
				.has_backend_neutral_meshes = false,
				.has_backend_neutral_textures = false,
				.has_backend_neutral_hud = false,
				.has_backend_neutral_world_renderer = true,
				.has_runtime_smoke_test = false } },
			RenderBackendDescriptor{ GraphicsApi::OpenGLCompatibility, "OpenGLCompatibility", false, true, RenderBackendMigrationStatus{
				.has_backend_neutral_meshes = true,
				.has_backend_neutral_textures = false,
				.has_backend_neutral_hud = false,
				.has_backend_neutral_world_renderer = false,
				.has_runtime_smoke_test = true } },
			RenderBackendDescriptor{ GraphicsApi::DirectX12, "DirectX12", false, false, RenderBackendMigrationStatus{} }
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
