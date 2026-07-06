#include "VulkanDebugLabels.h"

#include <string>

namespace ve::rendering
{
	void VulkanDebugLabels::Initialize(VkDevice device, bool enabled) noexcept
	{
		device_ = device;
		enabled_ = enabled;
	}

	void VulkanDebugLabels::Release() noexcept
	{
		device_ = VK_NULL_HANDLE;
		enabled_ = false;
	}

	bool VulkanDebugLabels::IsAvailable() const noexcept
	{
		return enabled_ && device_ != VK_NULL_HANDLE && vkSetDebugUtilsObjectNameEXT != nullptr;
	}

	bool VulkanDebugLabels::NameObject(VkObjectType type, std::uint64_t handle, std::string_view name) const
	{
		if (!IsAvailable() || handle == 0U || name.empty())
		{
			return false;
		}

		const std::string object_name{ name };
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = type,
			.objectHandle = handle,
			.pObjectName = object_name.c_str()
		};
		return vkSetDebugUtilsObjectNameEXT(device_, &name_info) == VK_SUCCESS;
	}

	std::uint64_t VulkanDispatchableObjectHandle(VkDevice handle) noexcept
	{
		return reinterpret_cast<std::uint64_t>(handle);
	}
}
