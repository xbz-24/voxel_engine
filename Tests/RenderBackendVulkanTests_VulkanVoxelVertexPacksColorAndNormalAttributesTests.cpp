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

TEST_CASE("vulkan voxel vertex packs color and normal attributes")
{
	const std::uint32_t white = ve::rendering::PackVoxelColor(1.0f, 1.0f, 1.0f, 1.0f);
	const std::uint32_t up = ve::rendering::PackVoxelNormal({ 0, 1, 0 });
	const std::uint32_t negative_x = ve::rendering::PackVoxelNormal({ -1, 0, 0 });
	const glm::vec3 unpacked_up = ve::rendering::UnpackVoxelNormal(up);
	const glm::vec3 unpacked_negative_x = ve::rendering::UnpackVoxelNormal(negative_x);

	CHECK(sizeof(ve::rendering::VoxelVertex) == 24U);
	CHECK(white == 0xffffffffU);
	CHECK((up & 0x0000ff00U) == 0x00007f00U);
	CHECK(unpacked_up.y == doctest::Approx(1.0f));
	CHECK(unpacked_negative_x.x == doctest::Approx(-1.0f));
}

