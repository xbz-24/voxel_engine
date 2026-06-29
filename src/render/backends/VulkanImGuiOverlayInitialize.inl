	bool VulkanImGuiOverlay::Initialize(VulkanBackend& backend, ve::engine::Window& window, VkRenderPass render_pass)
	{
		Release();
		device_ = backend.Device().Handle();
		if (device_ == VK_NULL_HANDLE || render_pass == VK_NULL_HANDLE) return false;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsDark();
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Initializing Vulkan ImGui GLFW backend with input callbacks");

		if (!ImGui_ImplGlfw_InitForVulkan(window.GetNativeWindow(), true))
		{
			Release();
			return false;
		}

		const std::uint32_t image_count = static_cast<std::uint32_t>(backend.Swapchain().Images().size());
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Initializing Vulkan ImGui renderer backend with swapchain images: " + std::to_string(image_count));
		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.ApiVersion = VK_API_VERSION_1_0;
		init_info.Instance = backend.Context().Instance();
		init_info.PhysicalDevice = backend.PhysicalDevice().Handle();
		init_info.Device = device_;
		init_info.QueueFamily = backend.PhysicalDevice().QueueFamilies().graphics_family;
		init_info.Queue = backend.Device().GraphicsQueue();
		init_info.RenderPass = render_pass;
		init_info.MinImageCount = std::min(std::max(image_count, 2u), 3u);
		init_info.ImageCount = image_count;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.DescriptorPoolSize = 64u;
		init_info.CheckVkResultFn = CheckVkResult;
		init_info.MinAllocationSize = 1024u * 1024u;
		if (!ImGui_ImplVulkan_Init(&init_info))
		{
			Release();
			return false;
		}

		initialized_ = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan ImGui overlay initialized");
		return true;
	}
