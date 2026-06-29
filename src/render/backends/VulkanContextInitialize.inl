	VulkanContext::~VulkanContext() { Release(); }

	bool VulkanContext::Initialize(const VulkanContextSettings& settings)
	{
		Release();
		if (volkInitialize() != VK_SUCCESS)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "volk initialization failed");
			return false;
		}
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		const std::vector<VkLayerProperties> available_layers = EnumerateInstanceLayers();
		const std::vector<VkExtensionProperties> available_extensions = EnumerateInstanceExtensions();
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, std::string("Available Vulkan layers: ") + JoinLayerNames(available_layers));

		LayerSelection enabled_layers = SelectLayers(settings, available_layers);
		std::vector<const char*> enabled_extensions = settings.required_extensions;
		const bool can_enable_debug_utils = ContainsExtension(available_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		if (settings.enable_debug_utils && can_enable_debug_utils)
		{
			AppendUniqueExtension(enabled_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		else if (settings.enable_debug_utils)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "VK_EXT_debug_utils is unavailable; Vulkan validation messages will not be hooked");
		}

		VE_LOG_CATEGORY_INFO(ve::log::category::Render, std::string("Enabled Vulkan layers: ") + JoinNames(enabled_layers.names));
		const VkApplicationInfo app_info = CreateApplicationInfo(settings.application_name.c_str());
		VkDebugUtilsMessengerCreateInfoEXT debug_info = CreateDebugMessengerInfo();
		const void* debug_info_ptr = settings.enable_debug_utils && can_enable_debug_utils ? &debug_info : nullptr;
		const VkInstanceCreateInfo instance_info = CreateInstanceInfo(app_info, enabled_layers.pointers, enabled_extensions, debug_info_ptr);
		if (vkCreateInstance(&instance_info, nullptr, &instance_) != VK_SUCCESS)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "Vulkan instance creation failed");
			return false;
		}

		volkLoadInstance(instance_);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Instance{ instance_ });
		if (settings.enable_debug_utils && can_enable_debug_utils)
		{
			if (vkCreateDebugUtilsMessengerEXT(instance_, &debug_info, nullptr, &debug_messenger_) != VK_SUCCESS)
			{
				VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Vulkan debug messenger creation failed");
			}
		}
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan instance initialized");
		return true;
	}
