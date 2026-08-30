#include <doctest/doctest.h>

#include "DirectX12Backend.h"
#include "OpenGLCompatibilityBackend.h"
#include "RenderBackendCatalog.h"
#include "RenderBackendFactory.h"
#include "RenderBackendSelector.h"
#include "TextureLoader.h"
#include "VulkanBackendSettings.h"
#include "VulkanChunkMeshTranslator.h"
#include "VulkanDebugLabels.h"
#include "VulkanGpuChunkRendererTypes.h"
#include "VulkanRenderView.h"
#include "VulkanSoftwareRasterizerColor.h"
#include "VulkanSoftwareVoxelRasterizerData.h"
#include "VulkanSwapchainChoices.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

TEST_CASE("vulkan render view exposes vulkan hpp handles")
{
	const vk::Extent2D extent{ 1280U, 720U };
	ve::engine::VulkanRenderView view({ vk::Device{}, vk::SwapchainKHR{}, extent });

	CHECK(view.Api() == ve::rendering::GraphicsApi::Vulkan);
	CHECK(view.Device() == vk::Device{});
	CHECK(view.Swapchain() == vk::SwapchainKHR{});
	CHECK(view.SwapchainExtent().width == 1280U);
	CHECK(view.SwapchainExtent().height == 720U);
	CHECK(view.Graphics() == nullptr);
	CHECK(view.Hud() == nullptr);
}

