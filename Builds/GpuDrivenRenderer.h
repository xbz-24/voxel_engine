#pragma once

#include "GpuDrivenDrawBuffer.h"

namespace ve::rendering
{
	/** Submits indexed geometry through Vulkan indirect draw commands. */
	class GpuDrivenRenderer
	{
	public:
		/** @param commands Commands copied to the indirect draw buffer. */
		void UploadCommands(std::span<const GpuDrawElementsCommand> commands);

		/** @param command_buffer Vulkan command buffer receiving draw commands. */
		void RecordIndexedIndirect(VkCommandBuffer command_buffer) const;

		/** @return Uploaded command count. */
		[[nodiscard]] ve::core::Index CommandCount() const noexcept;

	private:
		GpuDrivenDrawBuffer draw_buffer_;
	};
}
