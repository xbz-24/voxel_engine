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

TEST_CASE("vulkan voxel vertex packs color and normal attributes")
{
	const std::uint32_t white = ve::rendering::PackVoxelColor(1.0f, 1.0f, 1.0f, 1.0f);
	const std::uint32_t up = ve::rendering::PackVoxelNormal({ 0, 1, 0 });

	CHECK(sizeof(ve::rendering::VoxelVertex) == 24U);
	CHECK(white == 0xffffffffU);
	CHECK((up & 0x0000ff00U) == 0x00007f00U);
}

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
