#pragma once

#include "VulkanGpuChunkRendererState.h"
#include "VulkanGpuChunkRendererPipelineTypes.h"
#include "VulkanGpuChunkRendererTypes.h"

#include <filesystem>

class Camera;

namespace ve::rendering
{
	using VulkanGpuOverlayRecordCallback = void (*)(VkCommandBuffer, void*);

	class VulkanGpuChunkRendererResourceOperations : protected VulkanGpuChunkRendererState
	{
	protected:
		[[nodiscard]] bool CreateRenderPass();
		[[nodiscard]] bool CreatePipeline(const std::filesystem::path& shader_directory);
		[[nodiscard]] bool CreatePipelineLayout();
		[[nodiscard]] bool CreateShaderFrameResources(std::size_t frame_count);
		[[nodiscard]] bool CreateShaderDescriptorInfrastructure(std::size_t frame_count);
		[[nodiscard]] bool CreateMappedShaderFrames(std::size_t frame_count);
		[[nodiscard]] bool AllocateShaderDescriptorSets();
		[[nodiscard]] bool CreateShadowFrameResources(VulkanGpuShaderFrameResources& frame) const;
		[[nodiscard]] bool UpdateShaderFrameUniforms(
			std::size_t frame_index, const VulkanVoxelFrameUniforms& uniforms) const;
		[[nodiscard]] bool RecordShadowPass(VkCommandBuffer command_buffer, std::size_t frame_index) const;
		[[nodiscard]] bool RecordColorPass(
			VkCommandBuffer command_buffer,
			std::uint32_t image_index,
			std::size_t frame_index,
			const VulkanVoxelFrameUniforms& uniforms,
			const Camera& camera,
			VulkanGpuOverlayRecordCallback overlay_callback,
			void* overlay_user_data) const;
		[[nodiscard]] bool CreateGraphicsPipeline(
			VkShaderModule vertex_shader,
			VkShaderModule fragment_shader,
			const VulkanGpuGraphicsPipelineSettings& settings,
			VkPipeline& output_pipeline) const;
		[[nodiscard]] bool CreateSwapchainResources();
		[[nodiscard]] bool CreateDepthResources();
		[[nodiscard]] bool CreateFramebuffers();
		[[nodiscard]] bool CreateBuffer(
			VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer& buffer, VkDeviceMemory& memory) const;
		[[nodiscard]] bool CreateHostBuffer(
			VkDeviceSize size, VkBufferUsageFlags usage,
			VkBuffer& buffer, VkDeviceMemory& memory) const;
		[[nodiscard]] bool UploadDeviceLocalBuffer(
			const void* source, VkDeviceSize size, VkBufferUsageFlags usage,
			VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize& capacity) const;
		[[nodiscard]] bool CreateDeviceImage(
			VkImageCreateInfo image_info, VkDeviceMemory& memory, VkImage& image) const;
		[[nodiscard]] bool CopyToDeviceBuffer(
			VkDeviceMemory memory, const void* source, VkDeviceSize size) const;
		[[nodiscard]] bool RunImmediateCommands(
			void (*record)(VkCommandBuffer, void*), void* user_data) const;
		[[nodiscard]] VkShaderModule CreateShaderModule(const std::filesystem::path& path) const;
		void ReleaseSwapchainResources();
		void ReleasePipelineResources();
		void ReleaseRenderPassResources();
		void ReleaseShaderFrameResources();
		[[nodiscard]] static std::uint32_t FindMemoryType(
			VkPhysicalDevice physical_device,
			std::uint32_t type_filter,
			VkMemoryPropertyFlags properties);
	};
}
