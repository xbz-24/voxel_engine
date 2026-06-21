#pragma once

#include <volk.h>

#include <cstddef>
#include <cstdint>

namespace ve::rendering
{
	class VulkanBackend;

	class VulkanUploadBuffer
	{
	public:
		~VulkanUploadBuffer();

		[[nodiscard]] bool Ensure(VulkanBackend& backend, VkDeviceSize byte_size);
		[[nodiscard]] bool CopyFrom(VkDevice device, const void* source, std::size_t byte_count) const;
		void Release(VkDevice device);

		[[nodiscard]] VkBuffer Buffer() const noexcept { return buffer_; }
		[[nodiscard]] VkDeviceSize Size() const noexcept { return size_; }

	private:
		[[nodiscard]] bool Create(VulkanBackend& backend, VkDeviceSize byte_size);
		[[nodiscard]] static std::uint32_t FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties);

		VkBuffer buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory memory_ = VK_NULL_HANDLE;
		VkDeviceSize size_ = 0;
		void* mapped_memory_ = nullptr;
	};
}
