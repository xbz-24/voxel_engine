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

TEST_CASE("vulkan debug labels are safe before debug utils are available")
{
	ve::rendering::VulkanDebugLabels labels;

	CHECK(!labels.IsAvailable());
	CHECK(!labels.NameObject(VK_OBJECT_TYPE_DEVICE, 1U, "device"));
	labels.Initialize(VK_NULL_HANDLE, true);
	CHECK(!labels.IsAvailable());
	labels.Release();
	CHECK(!labels.IsAvailable());
	CHECK(ve::rendering::VulkanPipelineObjectHandle(VkPipeline{}) == 0U);
}
