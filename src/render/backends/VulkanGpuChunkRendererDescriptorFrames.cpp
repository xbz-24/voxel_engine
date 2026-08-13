#include "VulkanGpuChunkRenderer.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRendererResourceOperations::CreateMappedShaderFrames(
		std::size_t frame_count)
	{
		shader_frame_resources_.resize(frame_count);
		for (VulkanGpuShaderFrameResources& frame : shader_frame_resources_)
		{
			if (!CreateHostBuffer(
				VulkanByteSize(sizeof(VulkanVoxelFrameUniforms)),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				frame.uniform_buffer,
				frame.uniform_memory) ||
				vkMapMemory(
					device_,
					frame.uniform_memory,
					0u,
					VulkanByteSize(sizeof(VulkanVoxelFrameUniforms)),
					0u,
					&frame.mapped_uniform_data) != VK_SUCCESS ||
				!CreateShadowFrameResources(frame))
			{
				return false;
			}
		}
		return true;
	}
}
