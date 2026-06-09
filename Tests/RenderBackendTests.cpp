#include <doctest/doctest.h>

#include "DirectX12Backend.h"
#include "OpenGLCompatibilityBackend.h"
#include "RenderBackendCatalog.h"
#include "RenderBackendFactory.h"
#include "RenderBackendSelector.h"
#include "VulkanSwapchainChoices.h"

#include <array>
#include <string>

TEST_CASE("render backend selector keeps legacy renderer on opengl compatibility")
{
	const ve::rendering::RenderBackendConfiguration configuration{};

	const ve::rendering::GraphicsApi selected_api = ve::rendering::RenderBackendSelector::SelectApi(configuration, true);

	CHECK(selected_api == ve::rendering::GraphicsApi::OpenGLCompatibility);
	CHECK(std::string{ ve::rendering::RenderBackendSelector::Name(configuration.preferred_api) } == "Vulkan");
}

TEST_CASE("render backend selector defaults to vulkan when legacy rendering is gone")
{
	const ve::rendering::RenderBackendConfiguration configuration{};

	const ve::rendering::GraphicsApi selected_api = ve::rendering::RenderBackendSelector::SelectApi(configuration, false);

	CHECK(selected_api == ve::rendering::GraphicsApi::Vulkan);
}

TEST_CASE("render backend catalog exposes vulkan as the default api")
{
	const ve::rendering::RenderBackendDescriptor& backend = ve::rendering::RenderBackendCatalog::DefaultBackend();

	CHECK(backend.api == ve::rendering::GraphicsApi::Vulkan);
	CHECK(backend.is_default);
	CHECK(backend.is_implemented);
}

TEST_CASE("render backend factory creates every declared backend object")
{
	for (const ve::rendering::RenderBackendDescriptor& descriptor : ve::rendering::RenderBackendCatalog::Backends())
	{
		const std::unique_ptr<ve::rendering::RenderBackend> backend = ve::rendering::RenderBackendFactory::Create(descriptor.api);
		REQUIRE(backend != nullptr);
		CHECK(backend->Api() == descriptor.api);
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

TEST_CASE("vulkan swapchain choices prefer mailbox and srgb")
{
	const VkSurfaceFormatKHR fallback{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	const VkSurfaceFormatKHR preferred{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	const std::array formats{ fallback, preferred };
	const std::array modes{ VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_MAILBOX_KHR };

	CHECK(ve::rendering::ChooseSwapchainSurfaceFormat(formats).format == VK_FORMAT_B8G8R8A8_SRGB);
	CHECK(ve::rendering::ChooseSwapchainPresentMode(modes) == VK_PRESENT_MODE_MAILBOX_KHR);
}
