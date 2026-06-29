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
	CHECK(ve::rendering::ChooseSwapchainPresentMode(modes, true) == VK_PRESENT_MODE_FIFO_KHR);
	CHECK(ve::rendering::ChooseSwapchainPresentMode(low_latency_modes, true) == VK_PRESENT_MODE_FIFO_KHR);
}
