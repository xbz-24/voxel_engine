#pragma once

#include "AssetPaths.h"
#include "RenderApi.h"
#include "RenderView.h"

#include <memory>

namespace ve::rendering
{
	class VulkanBackend;
}

namespace ve::engine
{
	struct RenderViewCreateInfo
	{
		ve::rendering::GraphicsApi api = ve::rendering::GraphicsApi::Vulkan;
		const ve::assets::AssetPaths* asset_paths = nullptr;
		ve::rendering::VulkanBackend* vulkan_backend = nullptr;
	};

	/** Factory Method object that creates the render view matching the selected backend. */
	class RenderViewFactory
	{
	public:
		/** @param create_info Backend choice plus resources needed by that view. @return Render view, or null on invalid input. */
		[[nodiscard]] static std::unique_ptr<RenderView> Create(const RenderViewCreateInfo& create_info);
	};
}
