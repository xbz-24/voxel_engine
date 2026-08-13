#include "VulkanUploadBuffer.h"

#include "CoreTypes.h"
#include "VulkanBackend.h"

#include <cstring>

namespace ve::rendering
{
	VulkanUploadBuffer::~VulkanUploadBuffer()
	{
		Release(VK_NULL_HANDLE);
	}

	bool VulkanUploadBuffer::Ensure(VulkanBackend& backend, VkDeviceSize byte_size)
	{
		const VkDevice device = backend.Device().Handle();
		if (buffer_ != VK_NULL_HANDLE && size_ == byte_size) return true;
		Release(device);
		return Create(backend, byte_size);
	}

	bool VulkanUploadBuffer::CopyFrom(
		VkDevice device,
		const void* source,
		std::size_t byte_count) const
	{
		(void)device;
		if (buffer_ == VK_NULL_HANDLE || memory_ == VK_NULL_HANDLE || mapped_memory_ == nullptr ||
			byte_count > ve::core::ToIndex(size_)) return false;
		std::memcpy(mapped_memory_, source, byte_count);
		return true;
	}

	void VulkanUploadBuffer::Release(VkDevice device)
	{
		if (device != VK_NULL_HANDLE && memory_ != VK_NULL_HANDLE && mapped_memory_ != nullptr)
		{
			vkUnmapMemory(device, memory_);
		}
		if (device != VK_NULL_HANDLE && buffer_ != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(device, buffer_, nullptr);
		}
		if (device != VK_NULL_HANDLE && memory_ != VK_NULL_HANDLE)
		{
			vkFreeMemory(device, memory_, nullptr);
		}
		buffer_ = VK_NULL_HANDLE;
		memory_ = VK_NULL_HANDLE;
		size_ = 0;
		mapped_memory_ = nullptr;
	}
}
