#include "VulkanContext.h"

#include <array>
#include <cstdint>
#include <span>

namespace ve::rendering
{
	namespace
	{
		/** @param name Engine application name. @return Vulkan application metadata. */
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

		/** @param app_info App metadata. @param layers Optional layers. @param extensions Required extensions. @return Instance creation info. */
		VkInstanceCreateInfo CreateInstanceInfo(const VkApplicationInfo& app_info, std::span<const char* const> layers, std::span<const char* const> extensions) noexcept
		{
			VkInstanceCreateInfo info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
			info.pApplicationInfo = &app_info;
			info.enabledLayerCount = static_cast<std::uint32_t>(layers.size());
			info.ppEnabledLayerNames = layers.data();
			info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
			info.ppEnabledExtensionNames = extensions.data();
			return info;
		}
	}

	/** Releases the Vulkan instance. */
	VulkanContext::~VulkanContext() { Release(); }

	/** Initializes volk and creates a Vulkan instance. */
	bool VulkanContext::Initialize(const VulkanContextSettings& settings)
	{
		Release();
		if (volkInitialize() != VK_SUCCESS) return false;
		const auto validation_layers = std::array{ "VK_LAYER_KHRONOS_validation" };
		std::span<const char* const> layers{};
		if (settings.enable_validation_layers) layers = validation_layers;
		const VkApplicationInfo app_info = CreateApplicationInfo(settings.application_name.c_str());
		const VkInstanceCreateInfo instance_info = CreateInstanceInfo(app_info, layers, settings.required_extensions);
		if (vkCreateInstance(&instance_info, nullptr, &instance_) != VK_SUCCESS) return false;
		volkLoadInstance(instance_);
		return true;
	}

	/** Destroys the Vulkan instance. */
	void VulkanContext::Release()
	{
		if (instance_ != VK_NULL_HANDLE) vkDestroyInstance(instance_, nullptr);
		instance_ = VK_NULL_HANDLE;
	}

	/** Returns the Vulkan instance handle. */
	VkInstance VulkanContext::Instance() const noexcept { return instance_; }

	/** Returns the Vulkan-Hpp instance handle. */
	vk::Instance VulkanContext::CppInstance() const noexcept { return vk::Instance{ instance_ }; }

	/** Reports whether the context owns a Vulkan instance. */
	bool VulkanContext::IsInitialized() const noexcept { return instance_ != VK_NULL_HANDLE; }
}
