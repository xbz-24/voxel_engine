#pragma once

#include <volk.h>
#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

namespace ve::rendering
{
	struct VulkanContextSettings
	{
		std::string application_name = "VoxelEngine";
		bool enable_validation_layers = false;
		std::vector<const char*> required_extensions{};
	};

	/** Owns a Vulkan instance loaded through volk. */
	class VulkanContext
	{
	public:
		/** Releases the Vulkan instance when the context is destroyed. */
		~VulkanContext();

		/** @param settings Application name and validation preferences. @return True when Vulkan is ready. */
		[[nodiscard]] bool Initialize(const VulkanContextSettings& settings);

		/** Destroys the Vulkan instance and resets handles. */
		void Release();

		/** @return Vulkan instance handle, or VK_NULL_HANDLE when inactive. */
		[[nodiscard]] VkInstance Instance() const noexcept;

		/** @return C++ Vulkan-Hpp instance handle, or an empty handle when inactive. */
		[[nodiscard]] vk::Instance CppInstance() const noexcept;

		/** @return True when the instance was created successfully. */
		[[nodiscard]] bool IsInitialized() const noexcept;

	private:
		VkInstance instance_ = VK_NULL_HANDLE;
	};
}
