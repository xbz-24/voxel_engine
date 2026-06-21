#include <doctest/doctest.h>

#include "DirectX12Backend.h"
#include "OpenGLCompatibilityBackend.h"
#include "RenderBackendCatalog.h"
#include "RenderBackendFactory.h"
#include "RenderBackendSelector.h"
#include "VulkanChunkMeshTranslator.h"
#include "VulkanRenderView.h"
#include "VulkanSwapchainChoices.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

TEST_CASE("render backend selector uses the explicitly requested compatibility backend")
{
	const ve::rendering::RenderBackendConfiguration configuration{
		.preferred_api = ve::rendering::GraphicsApi::OpenGLCompatibility
	};

	const ve::rendering::GraphicsApi selected_api = ve::rendering::RenderBackendSelector::SelectApi(configuration);

	CHECK(selected_api == ve::rendering::GraphicsApi::OpenGLCompatibility);
}

TEST_CASE("render backend selector defaults to vulkan when legacy rendering is gone")
{
	const ve::rendering::RenderBackendConfiguration configuration{};

	const ve::rendering::GraphicsApi selected_api = ve::rendering::RenderBackendSelector::SelectApi(configuration);

	CHECK(selected_api == ve::rendering::GraphicsApi::Vulkan);
	CHECK(!configuration.allow_opengl_compatibility_fallback);
}

TEST_CASE("render backend catalog exposes vulkan as the default api")
{
	const ve::rendering::RenderBackendDescriptor& backend = ve::rendering::RenderBackendCatalog::DefaultBackend();

	CHECK(backend.api == ve::rendering::GraphicsApi::Vulkan);
	CHECK(backend.is_default);
	CHECK(backend.is_implemented);
	CHECK(backend.migration.has_backend_neutral_world_renderer);
	CHECK(!backend.migration.has_backend_neutral_hud);
}

TEST_CASE("render backend factory creates every declared backend object")
{
	for (const ve::rendering::RenderBackendDescriptor& descriptor : ve::rendering::RenderBackendCatalog::Backends())
	{
		const std::unique_ptr<ve::rendering::RenderBackend> backend = ve::rendering::RenderBackendFactory::Create(descriptor.api);
		REQUIRE(backend != nullptr);
		CHECK(backend->Api() == descriptor.api);
		CHECK(backend->MigrationStatus().has_runtime_smoke_test == descriptor.migration.has_runtime_smoke_test);
	}
}

TEST_CASE("opengl compatibility backend exposes conservative capabilities")
{
	const ve::rendering::OpenGLCompatibilityBackend backend;
	const ve::rendering::RenderBackendCapabilities capabilities = backend.Capabilities();

	CHECK(backend.Api() == ve::rendering::GraphicsApi::OpenGLCompatibility);
	CHECK(!capabilities.supports_compute);
	CHECK(!capabilities.supports_ray_tracing);
	CHECK(capabilities.is_available);
}

TEST_CASE("directx backend is declared but unavailable until implemented")
{
	const ve::rendering::DirectX12Backend backend;
	const ve::rendering::RenderBackendCapabilities capabilities = backend.Capabilities();

	CHECK(backend.Api() == ve::rendering::GraphicsApi::DirectX12);
	CHECK(!capabilities.is_available);
	CHECK(std::string{ backend.Name() } == "DirectX12");
}

TEST_CASE("vulkan swapchain choices prefer uncapped present and srgb")
{
	const VkSurfaceFormatKHR fallback{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	const VkSurfaceFormatKHR preferred{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	const std::array formats{ fallback, preferred };
	const std::array modes{ VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR };
	const std::array low_latency_modes{ VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_MAILBOX_KHR };

	CHECK(ve::rendering::ChooseSwapchainSurfaceFormat(formats).format == VK_FORMAT_B8G8R8A8_SRGB);
	CHECK(ve::rendering::ChooseSwapchainPresentMode(modes) == VK_PRESENT_MODE_IMMEDIATE_KHR);
	CHECK(ve::rendering::ChooseSwapchainPresentMode(low_latency_modes) == VK_PRESENT_MODE_MAILBOX_KHR);
}

TEST_CASE("vulkan render view exposes vulkan hpp handles")
{
	const vk::Extent2D extent{ 1280U, 720U };
	const ve::engine::VulkanRenderView view({ vk::Device{}, vk::SwapchainKHR{}, extent });

	CHECK(view.Api() == ve::rendering::GraphicsApi::Vulkan);
	CHECK(view.Device() == vk::Device{});
	CHECK(view.Swapchain() == vk::SwapchainKHR{});
	CHECK(view.SwapchainExtent().width == 1280U);
	CHECK(view.SwapchainExtent().height == 720U);
	CHECK(view.AsVulkanRenderView() == &view);
	CHECK(ve::engine::TryRenderViewCast<ve::engine::VulkanRenderView>(view) == &view);
	CHECK(view.AsOpenGLRenderView() == nullptr);
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
