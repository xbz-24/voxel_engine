#pragma once

#include <volk.h>

#include <cstdint>
#include <string_view>

namespace ve::rendering
{
	/** Centralizes optional VK_EXT_debug_utils object names. */
	class VulkanDebugLabels
	{
	public:
		/** @param device Logical device used by vkSetDebugUtilsObjectNameEXT. @param enabled Whether debug utils were requested. */
		void Initialize(VkDevice device, bool enabled) noexcept;

		/** Clears the logical device association. */
		void Release() noexcept;

		/** @return True when labels can be submitted to Vulkan. */
		[[nodiscard]] bool IsAvailable() const noexcept;

		/** @param type Vulkan object type. @param handle Raw Vulkan object handle. @param name Debug label. @return True when the name was submitted. */
		[[nodiscard]] bool NameObject(VkObjectType type, std::uint64_t handle, std::string_view name) const;

	private:
		VkDevice device_ = VK_NULL_HANDLE;
		bool enabled_ = false;
	};

	/** @param handle Dispatchable Vulkan handle. @return 64-bit debug-utils object handle. */
	[[nodiscard]] std::uint64_t VulkanDispatchableObjectHandle(VkDevice handle) noexcept;
}
