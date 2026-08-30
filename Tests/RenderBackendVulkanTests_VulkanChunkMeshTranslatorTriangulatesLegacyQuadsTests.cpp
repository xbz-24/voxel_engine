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

TEST_CASE("vulkan chunk mesh translator triangulates legacy quads")
{
	ve::world::mesh::ChunkMeshBuildResult mesh;
	mesh.vertices = {
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f }
	};
	mesh.batches = { ve::rendering::ChunkMeshBatch{ ve::rendering::TextureHandle{ 42u }, 0u, 4u } };

	const ve::rendering::VulkanChunkMeshPayload payload = ve::rendering::BuildVulkanChunkMeshPayload(mesh);

	CHECK(payload.vertices.size() == 4u);
	CHECK(payload.indices == std::vector<std::uint32_t>{ 0u, 1u, 2u, 0u, 2u, 3u });
	REQUIRE(payload.batches.size() == 1u);
	CHECK(payload.batches.front().texture == 42u);
	CHECK(payload.batches.front().first_index == 0u);
	CHECK(payload.batches.front().index_count == 6u);
	CHECK(payload.draw.index_count == 6u);
	CHECK(payload.draw.instance_count == 1u);
}

