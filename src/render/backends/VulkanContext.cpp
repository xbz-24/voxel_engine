#include "VulkanContext.h"

#include "Logger.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <vector>

namespace ve::rendering
{
	namespace
	{
		struct LayerSelection
		{
			std::vector<std::string> names;
			std::vector<const char*> pointers;
		};

		std::vector<VkLayerProperties> EnumerateInstanceLayers()
		{
			std::uint32_t layer_count = 0;
			if (vkEnumerateInstanceLayerProperties(&layer_count, nullptr) != VK_SUCCESS) return {};
			std::vector<VkLayerProperties> layers(layer_count);
			if (layer_count > 0 && vkEnumerateInstanceLayerProperties(&layer_count, layers.data()) != VK_SUCCESS) return {};
			return layers;
		}

		std::vector<VkExtensionProperties> EnumerateInstanceExtensions()
		{
			std::uint32_t extension_count = 0;
			if (vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr) != VK_SUCCESS) return {};
			std::vector<VkExtensionProperties> extensions(extension_count);
			if (extension_count > 0 && vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()) != VK_SUCCESS) return {};
			return extensions;
		}

		bool ContainsLayer(std::span<const VkLayerProperties> layers, const char* name) noexcept
		{
			return std::ranges::any_of(layers, [name](const VkLayerProperties& layer)
			{
				return std::strcmp(layer.layerName, name) == 0;
			});
		}

		bool ContainsExtension(std::span<const VkExtensionProperties> extensions, const char* name) noexcept
		{
			return std::ranges::any_of(extensions, [name](const VkExtensionProperties& extension)
			{
				return std::strcmp(extension.extensionName, name) == 0;
			});
		}

		bool ContainsName(std::span<const std::string> names, const char* name) noexcept
		{
			return std::ranges::any_of(names, [name](const std::string& layer_name)
			{
				return layer_name == name;
			});
		}

		std::string JoinLayerNames(std::span<const VkLayerProperties> layers)
		{
			std::string result;
			for (const VkLayerProperties& layer : layers)
			{
				if (!result.empty()) result += ", ";
				result += layer.layerName;
			}
			return result.empty() ? std::string{ "<none>" } : result;
		}

		std::string JoinNames(std::span<const std::string> names)
		{
			std::string result;
			for (const std::string& name : names)
			{
				if (!result.empty()) result += ", ";
				result += name;
			}
			return result.empty() ? std::string{ "<none>" } : result;
		}

		LayerSelection SelectLayers(const VulkanContextSettings& settings, std::span<const VkLayerProperties> available_layers)
		{
			LayerSelection selection;
			if (settings.enable_all_available_layers)
			{
				selection.names.reserve(available_layers.size() + settings.requested_layers.size());
				for (const VkLayerProperties& layer : available_layers)
				{
					selection.names.emplace_back(layer.layerName);
				}
			}

			if (settings.enable_validation_layers)
			{
				for (const char* requested_layer : settings.requested_layers)
				{
					if (ContainsLayer(available_layers, requested_layer))
					{
						if (!ContainsName(selection.names, requested_layer)) selection.names.emplace_back(requested_layer);
					}
					else
					{
						VE_LOG_CATEGORY_WARNING(ve::log::category::Render, std::string("Requested Vulkan layer is unavailable: ") + requested_layer);
					}
				}
			}

			selection.pointers.reserve(selection.names.size());
			for (const std::string& name : selection.names)
			{
				selection.pointers.push_back(name.c_str());
			}
			return selection;
		}

		void AppendUniqueExtension(std::vector<const char*>& extensions, const char* extension)
		{
			const auto found = std::ranges::find_if(extensions, [extension](const char* enabled)
			{
				return std::strcmp(enabled, extension) == 0;
			});
			if (found == extensions.end()) extensions.push_back(extension);
		}

		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
			VkDebugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void*)
		{
			const char* message = callback_data && callback_data->pMessage ? callback_data->pMessage : "<no Vulkan message>";
			const std::string formatted = std::string("Vulkan validation: ") + message;
			if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0)
			{
				ve::log::Write(ve::log::Level::Error, ve::log::category::Render, formatted);
			}
			else if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0)
			{
				ve::log::Write(ve::log::Level::Warning, ve::log::category::Render, formatted);
			}
			else
			{
				ve::log::Write(ve::log::Level::Debug, ve::log::category::Render, formatted);
			}
			return VK_FALSE;
		}

		VkDebugUtilsMessengerCreateInfoEXT CreateDebugMessengerInfo() noexcept
		{
			VkDebugUtilsMessengerCreateInfoEXT info{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
			info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			info.pfnUserCallback = DebugCallback;
			return info;
		}

		VkApplicationInfo CreateApplicationInfo(const char* name) noexcept
		{
			VkApplicationInfo info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
			info.pApplicationName = name;
			info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			info.pEngineName = "VoxelEngine";
			info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			info.apiVersion = VK_API_VERSION_1_3;
			return info;
		}

		VkInstanceCreateInfo CreateInstanceInfo(const VkApplicationInfo& app_info,
			std::span<const char* const> layers,
			std::span<const char* const> extensions,
			const void* next) noexcept
		{
			VkInstanceCreateInfo info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
			info.pNext = next;
			info.pApplicationInfo = &app_info;
			info.enabledLayerCount = static_cast<std::uint32_t>(layers.size());
			info.ppEnabledLayerNames = layers.data();
			info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
			info.ppEnabledExtensionNames = extensions.data();
			return info;
		}
	}

	VulkanContext::~VulkanContext() { Release(); }

	bool VulkanContext::Initialize(const VulkanContextSettings& settings)
	{
		Release();
		if (volkInitialize() != VK_SUCCESS)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "volk initialization failed");
			return false;
		}

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

	void VulkanContext::Release()
	{
		if (debug_messenger_ != VK_NULL_HANDLE)
		{
			vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
			debug_messenger_ = VK_NULL_HANDLE;
		}
		if (instance_ != VK_NULL_HANDLE) vkDestroyInstance(instance_, nullptr);
		instance_ = VK_NULL_HANDLE;
	}

	VkInstance VulkanContext::Instance() const noexcept { return instance_; }

	vk::Instance VulkanContext::CppInstance() const noexcept { return vk::Instance{ instance_ }; }

	bool VulkanContext::IsInitialized() const noexcept { return instance_ != VK_NULL_HANDLE; }
}
