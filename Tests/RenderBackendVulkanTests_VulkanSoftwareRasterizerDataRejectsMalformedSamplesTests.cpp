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

TEST_CASE("vulkan software rasterizer data rejects malformed samples")
{
	ve::rendering::VulkanRasterFrameWorldSnapshot snapshot;
	snapshot.width = 1;
	snapshot.height = 1;
	snapshot.depth = 1;

	CHECK(snapshot.GetBlock({ 0, 0, 0 }) == ve::blocks::BlockId::Air);
	snapshot.blocks = { ve::blocks::BlockId::Stone };
	CHECK(snapshot.GetBlock({ 0, 0, 0 }) == ve::blocks::BlockId::Stone);
	CHECK(snapshot.GetBlock({ 1, 0, 0 }) == ve::blocks::BlockId::Air);

	ve::rendering::VulkanRasterCpuTexture malformed_texture;
	malformed_texture.pixels = { 0x00abcdefU };
	malformed_texture.width = 0U;
	malformed_texture.height = 1U;
	CHECK(malformed_texture.Sample(0.5f, 0.5f) == ve::rendering::PackRgb({ 132, 132, 132 }));

	ve::rendering::VulkanRasterCpuTexture valid_texture;
	valid_texture.pixels = { 0x00123456U };
	valid_texture.width = 1U;
	valid_texture.height = 1U;
	CHECK(valid_texture.Sample(0.5f, 0.5f) == 0x00123456U);
}
