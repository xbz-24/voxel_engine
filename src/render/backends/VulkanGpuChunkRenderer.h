#pragma once

#include "Block.h"
#include "CoreTypes.h"

#include <volk.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

class Camera;

namespace ve::world
{
	class World;
}

namespace ve::rendering
{
	class VulkanBackend;

	class VulkanGpuChunkRenderer
	{
	public:
		[[nodiscard]] bool Initialize(VulkanBackend& backend,
			VkCommandPool command_pool,
			const std::filesystem::path& block_texture_directory,
			const std::filesystem::path& shader_directory);
		[[nodiscard]] bool EnsureWorldMesh(const ve::world::World& world);
		[[nodiscard]] bool Record(VkCommandBuffer command_buffer, std::uint32_t image_index, const Camera& camera);
		void Release();

		[[nodiscard]] bool IsInitialized() const noexcept;
		[[nodiscard]] std::uint32_t IndexCount() const noexcept;

	private:
		struct VoxelVertex
		{
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			float u = 0.0f;
			float v = 0.0f;
			float texture_layer = 0.0f;
			float light = 1.0f;
		};

		[[nodiscard]] bool CreateRenderPass();
		[[nodiscard]] bool CreatePipeline(const std::filesystem::path& shader_directory);
		[[nodiscard]] bool CreateDescriptors();
		[[nodiscard]] bool CreateTextureArray(const std::filesystem::path& block_texture_directory);
		[[nodiscard]] bool CreateSwapchainResources();
		[[nodiscard]] bool CreateDepthResources();
		[[nodiscard]] bool CreateFramebuffers();
		[[nodiscard]] bool UploadMeshBuffers(std::span<const VoxelVertex> vertices, std::span<const std::uint32_t> indices);
		[[nodiscard]] bool CreateHostBuffer(VkDeviceSize byte_size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory) const;
		[[nodiscard]] bool CreateDeviceImage(VkImageCreateInfo image_info, VkDeviceMemory& memory, VkImage& image) const;
		[[nodiscard]] bool CopyToDeviceBuffer(VkDeviceMemory memory, const void* source, VkDeviceSize byte_size) const;
		[[nodiscard]] bool RunImmediateCommands(void (*record)(VkCommandBuffer, void*), void* user_data) const;
		[[nodiscard]] VkShaderModule CreateShaderModule(const std::filesystem::path& path) const;
		void ReleaseSwapchainResources();
		void ReleaseMeshBuffers();
		void ReleaseTextureResources();
		void ReleasePipelineResources();
		void RebuildMesh(const ve::world::World& world, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;
		[[nodiscard]] std::uint32_t TextureLayer(ve::blocks::BlockId block, ve::blocks::BlockFace face) const noexcept;
		[[nodiscard]] static std::uint32_t FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties);

		VulkanBackend* backend_ = nullptr;
		VkDevice device_ = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
		VkCommandPool command_pool_ = VK_NULL_HANDLE;
		VkRenderPass render_pass_ = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
		VkPipeline pipeline_ = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptor_set_layout_ = VK_NULL_HANDLE;
		VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
		VkDescriptorSet descriptor_set_ = VK_NULL_HANDLE;
		VkSampler texture_sampler_ = VK_NULL_HANDLE;
		VkImage texture_image_ = VK_NULL_HANDLE;
		VkDeviceMemory texture_memory_ = VK_NULL_HANDLE;
		VkImageView texture_view_ = VK_NULL_HANDLE;
		VkImage depth_image_ = VK_NULL_HANDLE;
		VkDeviceMemory depth_memory_ = VK_NULL_HANDLE;
		VkImageView depth_view_ = VK_NULL_HANDLE;
		VkBuffer vertex_buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory vertex_memory_ = VK_NULL_HANDLE;
		VkBuffer index_buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory index_memory_ = VK_NULL_HANDLE;
		ve::core::DynamicArray<VkImageView> swapchain_image_views_;
		ve::core::DynamicArray<VkFramebuffer> framebuffers_;
		std::array<std::uint32_t, static_cast<std::size_t>(ve::blocks::BlockId::Count) * static_cast<std::size_t>(ve::blocks::BlockFace::Count)> texture_layers_{};
		VkExtent2D extent_{};
		VkFormat color_format_ = VK_FORMAT_UNDEFINED;
		VkFormat depth_format_ = VK_FORMAT_D32_SFLOAT;
		std::uint64_t mesh_revision_ = 0;
		std::uint32_t index_count_ = 0;
		bool mesh_valid_ = false;
		bool initialized_ = false;
	};
}
