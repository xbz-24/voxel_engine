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
	const std::uint32_t negative_x = ve::rendering::PackVoxelNormal({ -1, 0, 0 });
	const glm::vec3 unpacked_up = ve::rendering::UnpackVoxelNormal(up);
	const glm::vec3 unpacked_negative_x = ve::rendering::UnpackVoxelNormal(negative_x);

	CHECK(sizeof(ve::rendering::VoxelVertex) == 24U);
	CHECK(white == 0xffffffffU);
	CHECK((up & 0x0000ff00U) == 0x00007f00U);
	CHECK(unpacked_up.y == doctest::Approx(1.0f));
	CHECK(unpacked_negative_x.x == doctest::Approx(-1.0f));
}

TEST_CASE("vulkan voxel frame uniforms preserve the portable shader contract")
{
	ve::rendering::VoxelRenderStyle style{};
	style.sun_direction = { 1.0e30f, 0.0f, 0.0f };
	style.sun_intensity = 1.4f;
	style.exposure = 1.2f;
	style.fog_start_distance = 90.0f;
	style.fog_end_distance = 330.0f;
	style.cloud_coverage = 0.35f;
	style.cloud_density = 0.72f;
	style.surface_detail_strength = 1.3f;
	style.water_reflection_strength = 1.5f;
	style.ambient_occlusion_strength = 1.25f;
	style.aerial_perspective_strength = 0.85f;
	style.subsurface_scattering_strength = 1.45f;
	style.cloud_light_absorption = 0.75f;
	style.directional_shadow_strength = 1.1f;
	style.directional_shadow_softness = 0.65f;
	style.directional_shadow_distance = 192.0f;

