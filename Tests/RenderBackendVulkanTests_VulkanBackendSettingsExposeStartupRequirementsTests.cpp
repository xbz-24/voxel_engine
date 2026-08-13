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

TEST_CASE("vulkan backend settings expose startup requirements")
{
	const ve::rendering::VulkanBackendSettings settings{};

	CHECK(settings.physical_device.require_swapchain_support);
	REQUIRE(settings.device.required_extensions.size() == 1U);
	CHECK(std::string{ settings.device.required_extensions.front() } == VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	REQUIRE(settings.device.optional_extensions.size() == 1U);
	CHECK(std::string{ settings.device.optional_extensions.front() } == "VK_KHR_portability_subset");
	CHECK(settings.swapchain.width == 0);
	CHECK(settings.swapchain.height == 0);
	CHECK(settings.swapchain.extra_image_count == 1U);
	CHECK((settings.swapchain.image_usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) != 0);
	CHECK((settings.swapchain.image_usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) != 0);
}

