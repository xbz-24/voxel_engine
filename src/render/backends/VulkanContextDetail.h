#pragma once

#include "VulkanContext.h"

#include <span>
#include <string>
#include <vector>

namespace ve::rendering::detail
{
	struct LayerSelection
	{
		std::vector<std::string> names;
		std::vector<const char*> pointers;
	};

	[[nodiscard]] std::vector<VkLayerProperties> EnumerateInstanceLayers();
	[[nodiscard]] std::vector<VkExtensionProperties> EnumerateInstanceExtensions();
	[[nodiscard]] bool ContainsLayer(std::span<const VkLayerProperties> layers, const char* name) noexcept;
	[[nodiscard]] bool ContainsExtension(std::span<const VkExtensionProperties> extensions, const char* name) noexcept;
	[[nodiscard]] bool ContainsName(std::span<const std::string> names, const char* name) noexcept;
	[[nodiscard]] std::string JoinLayerNames(std::span<const VkLayerProperties> layers);
	[[nodiscard]] std::string JoinNames(std::span<const std::string> names);

	[[nodiscard]] LayerSelection SelectLayers(
		const VulkanContextSettings& settings,
		std::span<const VkLayerProperties> available_layers);
	void AppendUniqueExtension(std::vector<const char*>& extensions, const char* extension);

	[[nodiscard]] VkDebugUtilsMessengerCreateInfoEXT CreateDebugMessengerInfo() noexcept;
	[[nodiscard]] VkApplicationInfo CreateApplicationInfo(const char* name) noexcept;
	[[nodiscard]] VkInstanceCreateInfo CreateInstanceInfo(
		const VkApplicationInfo& app_info,
		std::span<const char* const> layers,
		std::span<const char* const> extensions,
		const void* next) noexcept;
}
