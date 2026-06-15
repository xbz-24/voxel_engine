#include "RenderViewFactory.h"

#include "OpenGLRenderView.h"
#include "VulkanBackend.h"
#include "VulkanRenderView.h"

namespace ve::engine
{
	namespace
	{
		/** @param extent Low-level Vulkan extent. @return Vulkan-Hpp extent with the same dimensions. */
		vk::Extent2D ToCppExtent(VkExtent2D extent) noexcept
		{
			return vk::Extent2D{ extent.width, extent.height };
		}

		/** @param backend Backend candidate. @return True when Vulkan view handles can be captured. */
		bool CanCreateVulkanView(const ve::rendering::VulkanBackend* backend) noexcept
		{
			return backend != nullptr && backend->IsInitialized();
		}
	}

	/** Creates the render view matching the selected graphics API. */
	std::unique_ptr<RenderView> RenderViewFactory::Create(const RenderViewCreateInfo& create_info)
	{
		switch (create_info.api)
		{
		case ve::rendering::GraphicsApi::OpenGLCompatibility:
			return create_info.asset_paths ? std::make_unique<OpenGLRenderView>(*create_info.asset_paths) : nullptr;
		case ve::rendering::GraphicsApi::Vulkan:
			if (!CanCreateVulkanView(create_info.vulkan_backend)) return nullptr;
			return std::make_unique<VulkanRenderView>(VulkanRenderViewCreateInfo{
				create_info.vulkan_backend->Device().CppHandle(),
				create_info.vulkan_backend->Swapchain().CppHandle(),
				ToCppExtent(create_info.vulkan_backend->Swapchain().Extent()) });
		case ve::rendering::GraphicsApi::DirectX12:
			return nullptr;
		}
		return nullptr;
	}
}
