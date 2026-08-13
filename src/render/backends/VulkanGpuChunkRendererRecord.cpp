#include "VulkanGpuChunkRenderer.h"

#include "Camera.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::Record(
		VkCommandBuffer command_buffer,
		std::uint32_t image_index,
		std::size_t frame_index,
		float elapsed_seconds,
		const Camera& camera,
		VulkanOverlayRecordCallback overlay_callback,
		void* overlay_user_data)
	{
		if (!initialized_ || image_index >= framebuffers_.size() ||
			frame_index >= shader_frame_resources_.size() ||
			voxel_pipeline_ == VK_NULL_HANDLE || sky_pipeline_ == VK_NULL_HANDLE)
		{
			return false;
		}
		const VulkanVoxelFrameUniforms uniforms = PackVulkanVoxelFrameUniforms(
			render_style_, camera.GetPosition(), camera.GetForward(), elapsed_seconds, extent_);
		return UpdateShaderFrameUniforms(frame_index, uniforms) &&
			RecordShadowPass(command_buffer, frame_index) &&
			RecordColorPass(command_buffer, image_index, frame_index,
				uniforms, camera, overlay_callback, overlay_user_data);
	}
}
