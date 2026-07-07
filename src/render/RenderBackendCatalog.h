#pragma once

#include "RenderApi.h"

#include <array>
#include <span>

namespace ve::rendering
{
	struct RenderBackendDescriptor
	{
		GraphicsApi api = GraphicsApi::Vulkan;
		const char* name = "Vulkan";
		bool is_default = false;
		bool is_implemented = false;
		RenderBackendMigrationStatus migration{};
	};

	/** Exposes the graphics APIs known by the engine. */
	class RenderBackendCatalog
	{
	public:
		/** @return Ordered backend descriptors with Vulkan first. */
		[[nodiscard]] static std::span<const RenderBackendDescriptor> Backends() noexcept;

		/** @return Descriptor for the default graphics backend. */
		[[nodiscard]] static const RenderBackendDescriptor& DefaultBackend() noexcept;

		/** @param api API to find. @return Descriptor for the API, or default descriptor when missing. */
		[[nodiscard]] static const RenderBackendDescriptor& Find(GraphicsApi api) noexcept;
	};
}
