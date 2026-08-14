#include "VulkanContext.h"
#include "VulkanContextDetail.h"

#include "CoreTypes.h"
#include "Logger.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <vector>

namespace ve::rendering::detail
{
	namespace
	{
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
			VkDebugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void*) noexcept
		{
			try
			{
				const char* message = callback_data && callback_data->pMessage ?
					callback_data->pMessage : "<no Vulkan message>";
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
			}
			catch (...)
			{
				// Driver callbacks must not observe C++ exceptions from diagnostics.
			}
			return VK_FALSE;
		}

		static_assert(noexcept(DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT{},
			VkDebugUtilsMessageTypeFlagsEXT{}, nullptr, nullptr)));
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
		info.applicationVersion = VK_MAKE_VERSION(
			VE_PROJECT_VERSION_MAJOR, VE_PROJECT_VERSION_MINOR, VE_PROJECT_VERSION_PATCH);
		info.pEngineName = "VoxelEngine";
		info.engineVersion = info.applicationVersion;
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
		info.enabledLayerCount = ve::core::ToU32(layers.size());
		info.ppEnabledLayerNames = layers.data();
		info.enabledExtensionCount = ve::core::ToU32(extensions.size());
		info.ppEnabledExtensionNames = extensions.data();
		return info;
	}
}
