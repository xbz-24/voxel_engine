#pragma once

#include "CoreTypes.h"

#include <volk.h>
#include <cstdint>
#include <span>

namespace ve::rendering
{
	/** Vulkan-compatible indirect draw command for vkCmdDrawIndexedIndirect. */
	struct GpuDrawElementsCommand
	{
		std::uint32_t index_count = 0;
		std::uint32_t instance_count = 1;
		std::uint32_t first_index = 0;
		std::int32_t vertex_offset = 0;
		std::uint32_t first_instance = 0;
	};

	/** Owns Vulkan indirect draw commands before they are uploaded or recorded. */
	class GpuDrivenDrawBuffer
	{
	public:
		/** @param commands Draw commands converted to Vulkan indirect commands. */
		void Upload(std::span<const GpuDrawElementsCommand> commands);

		/** Clears CPU-side commands and drops the optional Vulkan buffer handle. */
		void Release();

		/** @param buffer Vulkan buffer containing the uploaded command stream. */
		void SetDeviceBuffer(VkBuffer buffer) noexcept;

		/** @return Vulkan buffer used for vkCmdDrawIndexedIndirect, or null. */
		[[nodiscard]] VkBuffer DeviceBuffer() const noexcept;

		/** @return CPU-side commands used by fallback command recording. */
		[[nodiscard]] std::span<const VkDrawIndexedIndirectCommand> Commands() const noexcept;

		/** @return Number of indirect commands available for submission. */
		[[nodiscard]] ve::core::Index CommandCount() const noexcept;

	private:
		VkBuffer device_buffer_ = VK_NULL_HANDLE;
		ve::core::DynamicArray<VkDrawIndexedIndirectCommand> commands_;
	};
}
