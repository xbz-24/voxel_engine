#include "VulkanGpuChunkRenderer.h"

#include "BlockDefinitions.h"
#include "Camera.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "World.h"

#include <stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <glm/ext.hpp>
#include <string>
#include <unordered_map>

namespace ve::rendering
{
	namespace
	{
		struct TextureUploadContext
		{
			VkImage image = VK_NULL_HANDLE;
			VkBuffer staging = VK_NULL_HANDLE;
			std::uint32_t width = 0;
			std::uint32_t height = 0;
			std::uint32_t layers = 0;
		};

		struct BufferCopyContext
		{
			VkBuffer source = VK_NULL_HANDLE;
			VkBuffer destination = VK_NULL_HANDLE;
			VkDeviceSize byte_size = 0;
		};

		struct BlockFaceGeometry
		{
			ve::blocks::BlockFace face;
			glm::ivec3 neighbor_offset;
			std::array<glm::vec3, 4> corners;
			float light;
		};

		const std::array kFaces{
			BlockFaceGeometry{ ve::blocks::BlockFace::Top, { 0, 1, 0 }, { glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 1.0f } }, 1.00f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Bottom, { 0, -1, 0 }, { glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f } }, 0.52f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Front, { 0, 0, 1 }, { glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 1.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 1.0f } }, 0.86f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Back, { 0, 0, -1 }, { glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 0.0f } }, 0.72f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Left, { -1, 0, 0 }, { glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f } }, 0.78f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Right, { 1, 0, 0 }, { glm::vec3{ 1.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 1.0f } }, 0.82f }
		};

		const std::array kFaceUvs{
			glm::vec2{ 0.0f, 1.0f },
			glm::vec2{ 1.0f, 1.0f },
			glm::vec2{ 1.0f, 0.0f },
			glm::vec2{ 0.0f, 0.0f }
		};

		std::size_t FaceIndex(ve::blocks::BlockId block, ve::blocks::BlockFace face) noexcept
		{
			return (static_cast<std::size_t>(block) * static_cast<std::size_t>(ve::blocks::BlockFace::Count)) +
				static_cast<std::size_t>(face);
		}

		bool IsRenderableBlock(ve::blocks::BlockId block) noexcept
		{
			return block != ve::blocks::BlockId::Air &&
				static_cast<std::size_t>(block) < static_cast<std::size_t>(ve::blocks::BlockId::Count);
		}

		bool OccludesNeighborFaces(ve::blocks::BlockId block) noexcept
		{
			if (block == ve::blocks::BlockId::Glass ||
				block == ve::blocks::BlockId::Water ||
				block == ve::blocks::BlockId::OakLeaves ||
				block == ve::blocks::BlockId::BirchLeaves ||
				block == ve::blocks::BlockId::CherryLeaves)
			{
				return false;
			}
			const std::size_t index = static_cast<std::size_t>(block);
			return index < static_cast<std::size_t>(ve::blocks::BlockId::Count) &&
				ve::blocks::BuiltInBlockDefinitions[index].isSolid;
		}

		std::vector<char> ReadBinaryFile(const std::filesystem::path& path)
		{
			std::ifstream file(path, std::ios::ate | std::ios::binary);
			if (!file) return {};
			const std::streamsize size = file.tellg();
			if (size <= 0) return {};
			std::vector<char> bytes(static_cast<std::size_t>(size));
			file.seekg(0);
			file.read(bytes.data(), size);
			return bytes;
		}

		std::vector<std::uint8_t> BuildFallbackTexture(std::uint32_t width, std::uint32_t height)
		{
			std::vector<std::uint8_t> pixels(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u);
			for (std::uint32_t y = 0; y < height; ++y)
			{
				for (std::uint32_t x = 0; x < width; ++x)
				{
					const bool bright = ((x / 4u) + (y / 4u)) % 2u == 0u;
					const std::size_t index = (static_cast<std::size_t>(y) * width + x) * 4u;
					pixels[index] = bright ? 210u : 90u;
					pixels[index + 1u] = bright ? 80u : 40u;
					pixels[index + 2u] = bright ? 210u : 90u;
					pixels[index + 3u] = 255u;
				}
			}
			return pixels;
		}

		void RecordTextureUpload(VkCommandBuffer command_buffer, void* user_data)
		{
			const auto& context = *static_cast<const TextureUploadContext*>(user_data);
			VkImageMemoryBarrier to_transfer{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
			to_transfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			to_transfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			to_transfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			to_transfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			to_transfer.image = context.image;
			to_transfer.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			to_transfer.subresourceRange.levelCount = 1u;
			to_transfer.subresourceRange.layerCount = context.layers;
			to_transfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
				0, 0, nullptr, 0, nullptr, 1, &to_transfer);

			VkBufferImageCopy copy_region{};
			copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copy_region.imageSubresource.layerCount = context.layers;
			copy_region.imageExtent = { context.width, context.height, 1u };
			vkCmdCopyBufferToImage(command_buffer, context.staging, context.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

			VkImageMemoryBarrier to_shader{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
			to_shader.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			to_shader.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			to_shader.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			to_shader.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			to_shader.image = context.image;
			to_shader.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			to_shader.subresourceRange.levelCount = 1u;
			to_shader.subresourceRange.layerCount = context.layers;
			to_shader.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			to_shader.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0, 0, nullptr, 0, nullptr, 1, &to_shader);
		}

		void RecordBufferCopy(VkCommandBuffer command_buffer, void* user_data)
		{
			const auto& context = *static_cast<const BufferCopyContext*>(user_data);
			VkBufferCopy copy{};
			copy.size = context.byte_size;
			vkCmdCopyBuffer(command_buffer, context.source, context.destination, 1u, &copy);
		}
	}

	bool VulkanGpuChunkRenderer::Initialize(VulkanBackend& backend,
		VkCommandPool command_pool,
		const std::filesystem::path& block_texture_directory,
		const std::filesystem::path& shader_directory)
	{
		Release();
		backend_ = &backend;
		device_ = backend.Device().Handle();
		physical_device_ = backend.PhysicalDevice().Handle();
		command_pool_ = command_pool;
		if (device_ == VK_NULL_HANDLE || physical_device_ == VK_NULL_HANDLE || command_pool_ == VK_NULL_HANDLE) return false;
		if (!CreateDescriptors() || !CreateTextureArray(block_texture_directory) || !CreateRenderPass() ||
			!CreatePipeline(shader_directory) || !CreateSwapchainResources())
		{
			Release();
			return false;
		}
		initialized_ = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan GPU chunk renderer initialized");
		return true;
	}

	bool VulkanGpuChunkRenderer::IsInitialized() const noexcept { return initialized_; }

	std::uint32_t VulkanGpuChunkRenderer::IndexCount() const noexcept { return index_count_; }

	VkRenderPass VulkanGpuChunkRenderer::RenderPass() const noexcept { return render_pass_; }

	std::uint32_t VulkanGpuChunkRenderer::FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memory_properties{};
		vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
		for (std::uint32_t index = 0; index < memory_properties.memoryTypeCount; ++index)
		{
			const bool is_allowed = (type_filter & (1u << index)) != 0u;
			const bool has_properties = (memory_properties.memoryTypes[index].propertyFlags & properties) == properties;
			if (is_allowed && has_properties) return index;
		}
		return UINT32_MAX;
	}

	bool VulkanGpuChunkRenderer::CreateBuffer(VkDeviceSize byte_size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& memory) const
	{
		VkBufferCreateInfo buffer_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.size = byte_size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS) return false;

		VkMemoryRequirements requirements{};
		vkGetBufferMemoryRequirements(device_, buffer, &requirements);
		const std::uint32_t memory_type = FindMemoryType(physical_device_, requirements.memoryTypeBits, properties);
		if (memory_type == UINT32_MAX) return false;

		VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocate_info.allocationSize = requirements.size;
		allocate_info.memoryTypeIndex = memory_type;
		return vkAllocateMemory(device_, &allocate_info, nullptr, &memory) == VK_SUCCESS &&
			vkBindBufferMemory(device_, buffer, memory, 0) == VK_SUCCESS;
	}

	bool VulkanGpuChunkRenderer::CreateHostBuffer(VkDeviceSize byte_size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory) const
	{
		return CreateBuffer(byte_size,
			usage,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			buffer,
			memory);
	}

	bool VulkanGpuChunkRenderer::CopyToDeviceBuffer(VkDeviceMemory memory, const void* source, VkDeviceSize byte_size) const
	{
		void* mapped = nullptr;
		if (vkMapMemory(device_, memory, 0, byte_size, 0, &mapped) != VK_SUCCESS) return false;
		std::memcpy(mapped, source, static_cast<std::size_t>(byte_size));
		vkUnmapMemory(device_, memory);
		return true;
	}

	bool VulkanGpuChunkRenderer::UploadDeviceLocalBuffer(const void* source,
		VkDeviceSize byte_size,
		VkBufferUsageFlags usage,
		VkBuffer& buffer,
		VkDeviceMemory& memory) const
	{
		VkBuffer staging = VK_NULL_HANDLE;
		VkDeviceMemory staging_memory = VK_NULL_HANDLE;
		if (!CreateHostBuffer(byte_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging, staging_memory) ||
			!CopyToDeviceBuffer(staging_memory, source, byte_size) ||
			!CreateBuffer(byte_size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory))
		{
			if (staging != VK_NULL_HANDLE) vkDestroyBuffer(device_, staging, nullptr);
			if (staging_memory != VK_NULL_HANDLE) vkFreeMemory(device_, staging_memory, nullptr);
			return false;
		}

		BufferCopyContext context{ staging, buffer, byte_size };
		const bool copied = RunImmediateCommands(RecordBufferCopy, &context);
		vkDestroyBuffer(device_, staging, nullptr);
		vkFreeMemory(device_, staging_memory, nullptr);
		return copied;
	}

	bool VulkanGpuChunkRenderer::CreateDeviceImage(VkImageCreateInfo image_info, VkDeviceMemory& memory, VkImage& image) const
	{
		if (vkCreateImage(device_, &image_info, nullptr, &image) != VK_SUCCESS) return false;
		VkMemoryRequirements requirements{};
		vkGetImageMemoryRequirements(device_, image, &requirements);
		const std::uint32_t memory_type = FindMemoryType(physical_device_, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (memory_type == UINT32_MAX) return false;
		VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocate_info.allocationSize = requirements.size;
		allocate_info.memoryTypeIndex = memory_type;
		return vkAllocateMemory(device_, &allocate_info, nullptr, &memory) == VK_SUCCESS &&
			vkBindImageMemory(device_, image, memory, 0) == VK_SUCCESS;
	}

	bool VulkanGpuChunkRenderer::RunImmediateCommands(void (*record)(VkCommandBuffer, void*), void* user_data) const
	{
		VkCommandBuffer command_buffer = VK_NULL_HANDLE;
		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.commandPool = command_pool_;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = 1u;
		if (vkAllocateCommandBuffers(device_, &allocate_info, &command_buffer) != VK_SUCCESS) return false;

		VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) return false;
		record(command_buffer, user_data);
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) return false;

		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.commandBufferCount = 1u;
		submit_info.pCommandBuffers = &command_buffer;
		const bool submitted = vkQueueSubmit(backend_->Device().GraphicsQueue(), 1u, &submit_info, VK_NULL_HANDLE) == VK_SUCCESS;
		if (submitted) vkQueueWaitIdle(backend_->Device().GraphicsQueue());
		vkFreeCommandBuffers(device_, command_pool_, 1u, &command_buffer);
		return submitted;
	}

	bool VulkanGpuChunkRenderer::CreateDescriptors()
	{
		VkDescriptorSetLayoutBinding texture_binding{};
		texture_binding.binding = 0u;
		texture_binding.descriptorCount = 1u;
		texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layout_info.bindingCount = 1u;
		layout_info.pBindings = &texture_binding;
		if (vkCreateDescriptorSetLayout(device_, &layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS) return false;

		VkDescriptorPoolSize pool_size{};
		pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_size.descriptorCount = 1u;
		VkDescriptorPoolCreateInfo pool_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		pool_info.maxSets = 1u;
		pool_info.poolSizeCount = 1u;
		pool_info.pPoolSizes = &pool_size;
		if (vkCreateDescriptorPool(device_, &pool_info, nullptr, &descriptor_pool_) != VK_SUCCESS) return false;

		VkDescriptorSetAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocate_info.descriptorPool = descriptor_pool_;
		allocate_info.descriptorSetCount = 1u;
		allocate_info.pSetLayouts = &descriptor_set_layout_;
		return vkAllocateDescriptorSets(device_, &allocate_info, &descriptor_set_) == VK_SUCCESS;
	}

	bool VulkanGpuChunkRenderer::CreateTextureArray(const std::filesystem::path& block_texture_directory)
	{
		std::uint32_t texture_width = 0u;
		std::uint32_t texture_height = 0u;
		std::vector<std::uint8_t> pixels;
		std::unordered_map<std::string, std::uint32_t> layer_by_file;

		const auto ensure_fallback = [&pixels, &texture_width, &texture_height]
		{
			if (texture_width == 0u || texture_height == 0u)
			{
				texture_width = 16u;
				texture_height = 16u;
			}
			if (pixels.empty())
			{
				pixels = BuildFallbackTexture(texture_width, texture_height);
			}
		};

		const auto append_texture = [&](const char* file_name) -> std::uint32_t
		{
			if (file_name == nullptr) return 0u;
			if (const auto found = layer_by_file.find(file_name); found != layer_by_file.end()) return found->second;

			int width = 0;
			int height = 0;
			int channels = 0;
			const std::filesystem::path path = block_texture_directory / file_name;
			stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			if (data == nullptr || width <= 0 || height <= 0)
			{
				if (data != nullptr) stbi_image_free(data);
				return 0u;
			}

			const std::uint32_t loaded_width = static_cast<std::uint32_t>(width);
			const std::uint32_t loaded_height = static_cast<std::uint32_t>(height);
			if (texture_width == 0u || texture_height == 0u)
			{
				texture_width = loaded_width;
				texture_height = loaded_height;
				ensure_fallback();
			}
			if (loaded_width != texture_width || loaded_height != texture_height)
			{
				stbi_image_free(data);
				return 0u;
			}

			const std::uint32_t layer = static_cast<std::uint32_t>(pixels.size() / (static_cast<std::size_t>(texture_width) * texture_height * 4u));
			const std::size_t byte_count = static_cast<std::size_t>(texture_width) * static_cast<std::size_t>(texture_height) * 4u;
			pixels.insert(pixels.end(), data, data + byte_count);
			stbi_image_free(data);
			layer_by_file.emplace(file_name, layer);
			return layer;
		};

		texture_layers_.fill(0u);
		for (const ve::blocks::BlockDefinition& definition : ve::blocks::BuiltInBlockDefinitions)
		{
			const std::array files{
				definition.textures.top,
				definition.textures.bottom,
				definition.textures.front,
				definition.textures.back,
				definition.textures.left,
				definition.textures.right
			};
			for (std::size_t face = 0; face < files.size(); ++face)
			{
				texture_layers_[FaceIndex(definition.id, static_cast<ve::blocks::BlockFace>(face))] = append_texture(files[face]);
			}
		}
		ensure_fallback();

		const std::uint32_t layer_count = static_cast<std::uint32_t>(pixels.size() / (static_cast<std::size_t>(texture_width) * texture_height * 4u));
		VkBuffer staging = VK_NULL_HANDLE;
		VkDeviceMemory staging_memory = VK_NULL_HANDLE;
		const VkDeviceSize byte_size = static_cast<VkDeviceSize>(pixels.size());
		if (!CreateHostBuffer(byte_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging, staging_memory) ||
			!CopyToDeviceBuffer(staging_memory, pixels.data(), byte_size))
		{
			if (staging != VK_NULL_HANDLE) vkDestroyBuffer(device_, staging, nullptr);
			if (staging_memory != VK_NULL_HANDLE) vkFreeMemory(device_, staging_memory, nullptr);
			return false;
		}

		VkImageCreateInfo image_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		image_info.extent = { texture_width, texture_height, 1u };
		image_info.mipLevels = 1u;
		image_info.arrayLayers = layer_count;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (!CreateDeviceImage(image_info, texture_memory_, texture_image_))
		{
			vkDestroyBuffer(device_, staging, nullptr);
			vkFreeMemory(device_, staging_memory, nullptr);
			return false;
		}

		TextureUploadContext context{ texture_image_, staging, texture_width, texture_height, layer_count };
		const bool uploaded = RunImmediateCommands(RecordTextureUpload, &context);
		vkDestroyBuffer(device_, staging, nullptr);
		vkFreeMemory(device_, staging_memory, nullptr);
		if (!uploaded) return false;

		VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		view_info.image = texture_image_;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.levelCount = 1u;
		view_info.subresourceRange.layerCount = layer_count;
		if (vkCreateImageView(device_, &view_info, nullptr, &texture_view_) != VK_SUCCESS) return false;

		VkSamplerCreateInfo sampler_info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		sampler_info.magFilter = VK_FILTER_NEAREST;
		sampler_info.minFilter = VK_FILTER_NEAREST;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.maxLod = 0.0f;
		if (vkCreateSampler(device_, &sampler_info, nullptr, &texture_sampler_) != VK_SUCCESS) return false;

		VkDescriptorImageInfo image_descriptor{};
		image_descriptor.sampler = texture_sampler_;
		image_descriptor.imageView = texture_view_;
		image_descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		write.dstSet = descriptor_set_;
		write.dstBinding = 0u;
		write.descriptorCount = 1u;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &image_descriptor;
		vkUpdateDescriptorSets(device_, 1u, &write, 0u, nullptr);
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Loaded Vulkan GPU block texture array layers: " + std::to_string(layer_count));
		return true;
	}

	bool VulkanGpuChunkRenderer::CreateRenderPass()
	{
		color_format_ = backend_->Swapchain().ImageFormat();
		VkAttachmentDescription color_attachment{};
		color_attachment.format = color_format_;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depth_attachment{};
		depth_attachment.format = depth_format_;
		depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		const VkAttachmentReference color_reference{ 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		const VkAttachmentReference depth_reference{ 1u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1u;
		subpass.pColorAttachments = &color_reference;
		subpass.pDepthStencilAttachment = &depth_reference;

		std::array attachments{ color_attachment, depth_attachment };
		VkRenderPassCreateInfo render_pass_info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		render_pass_info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
		render_pass_info.pAttachments = attachments.data();
		render_pass_info.subpassCount = 1u;
		render_pass_info.pSubpasses = &subpass;
		return vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) == VK_SUCCESS;
	}

	VkShaderModule VulkanGpuChunkRenderer::CreateShaderModule(const std::filesystem::path& path) const
	{
		const std::vector<char> code = ReadBinaryFile(path);
		if (code.empty() || code.size() % sizeof(std::uint32_t) != 0u) return VK_NULL_HANDLE;
		VkShaderModuleCreateInfo create_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const std::uint32_t*>(code.data());
		VkShaderModule module = VK_NULL_HANDLE;
		if (vkCreateShaderModule(device_, &create_info, nullptr, &module) != VK_SUCCESS) return VK_NULL_HANDLE;
		return module;
	}

	bool VulkanGpuChunkRenderer::CreatePipeline(const std::filesystem::path& shader_directory)
	{
		const VkShaderModule vertex_shader = CreateShaderModule(shader_directory / "voxel_chunk.vert.spv");
		const VkShaderModule fragment_shader = CreateShaderModule(shader_directory / "voxel_chunk.frag.spv");
		if (vertex_shader == VK_NULL_HANDLE || fragment_shader == VK_NULL_HANDLE) return false;

		std::array shader_stages{
			VkPipelineShaderStageCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
			VkPipelineShaderStageCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO }
		};
		shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shader_stages[0].module = vertex_shader;
		shader_stages[0].pName = "main";
		shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shader_stages[1].module = fragment_shader;
		shader_stages[1].pName = "main";

		VkVertexInputBindingDescription binding{};
		binding.binding = 0u;
		binding.stride = sizeof(VoxelVertex);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		std::array attributes{
			VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, static_cast<std::uint32_t>(offsetof(VoxelVertex, x)) },
			VkVertexInputAttributeDescription{ 1u, 0u, VK_FORMAT_R32G32_SFLOAT, static_cast<std::uint32_t>(offsetof(VoxelVertex, u)) },
			VkVertexInputAttributeDescription{ 2u, 0u, VK_FORMAT_R32_SFLOAT, static_cast<std::uint32_t>(offsetof(VoxelVertex, texture_layer)) },
			VkVertexInputAttributeDescription{ 3u, 0u, VK_FORMAT_R32_SFLOAT, static_cast<std::uint32_t>(offsetof(VoxelVertex, light)) }
		};
		VkPipelineVertexInputStateCreateInfo vertex_input{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		vertex_input.vertexBindingDescriptionCount = 1u;
		vertex_input.pVertexBindingDescriptions = &binding;
		vertex_input.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attributes.size());
		vertex_input.pVertexAttributeDescriptions = attributes.data();

		VkPipelineInputAssemblyStateCreateInfo assembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkPipelineViewportStateCreateInfo viewport_state{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewport_state.viewportCount = 1u;
		viewport_state.scissorCount = 1u;
		std::array dynamic_states{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamic_state{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
		dynamic_state.pDynamicStates = dynamic_states.data();

		VkPipelineRasterizationStateCreateInfo rasterization{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterization.polygonMode = VK_POLYGON_MODE_FILL;
		rasterization.cullMode = VK_CULL_MODE_NONE;
		rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterization.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisample{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depth{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		depth.depthTestEnable = VK_TRUE;
		depth.depthWriteEnable = VK_TRUE;
		depth.depthCompareOp = VK_COMPARE_OP_LESS;

		VkPipelineColorBlendAttachmentState blend_attachment{};
		blend_attachment.blendEnable = VK_TRUE;
		blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
		blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		VkPipelineColorBlendStateCreateInfo blend{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		blend.attachmentCount = 1u;
		blend.pAttachments = &blend_attachment;

		VkPushConstantRange push_constants{};
		push_constants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		push_constants.size = sizeof(glm::mat4);
		VkPipelineLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		layout_info.setLayoutCount = 1u;
		layout_info.pSetLayouts = &descriptor_set_layout_;
		layout_info.pushConstantRangeCount = 1u;
		layout_info.pPushConstantRanges = &push_constants;
		if (vkCreatePipelineLayout(device_, &layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) return false;

		VkGraphicsPipelineCreateInfo pipeline_info{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipeline_info.stageCount = static_cast<std::uint32_t>(shader_stages.size());
		pipeline_info.pStages = shader_stages.data();
		pipeline_info.pVertexInputState = &vertex_input;
		pipeline_info.pInputAssemblyState = &assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterization;
		pipeline_info.pMultisampleState = &multisample;
		pipeline_info.pDepthStencilState = &depth;
		pipeline_info.pColorBlendState = &blend;
		pipeline_info.pDynamicState = &dynamic_state;
		pipeline_info.layout = pipeline_layout_;
		pipeline_info.renderPass = render_pass_;
		const bool created = vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1u, &pipeline_info, nullptr, &pipeline_) == VK_SUCCESS;
		vkDestroyShaderModule(device_, vertex_shader, nullptr);
		vkDestroyShaderModule(device_, fragment_shader, nullptr);
		return created;
	}

	bool VulkanGpuChunkRenderer::CreateSwapchainResources()
	{
		ReleaseSwapchainResources();
		extent_ = backend_->Swapchain().Extent();
		if (extent_.width == 0u || extent_.height == 0u) return false;
		const auto& images = backend_->Swapchain().Images();
		swapchain_image_views_.resize(images.size());
		for (std::size_t index = 0; index < images.size(); ++index)
		{
			VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			view_info.image = images[index];
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = color_format_;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.levelCount = 1u;
			view_info.subresourceRange.layerCount = 1u;
			if (vkCreateImageView(device_, &view_info, nullptr, &swapchain_image_views_[index]) != VK_SUCCESS) return false;
		}
		return CreateDepthResources() && CreateFramebuffers();
	}

	bool VulkanGpuChunkRenderer::CreateDepthResources()
	{
		VkImageCreateInfo image_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = depth_format_;
		image_info.extent = { extent_.width, extent_.height, 1u };
		image_info.mipLevels = 1u;
		image_info.arrayLayers = 1u;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (!CreateDeviceImage(image_info, depth_memory_, depth_image_)) return false;

		VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		view_info.image = depth_image_;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = depth_format_;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		view_info.subresourceRange.levelCount = 1u;
		view_info.subresourceRange.layerCount = 1u;
		return vkCreateImageView(device_, &view_info, nullptr, &depth_view_) == VK_SUCCESS;
	}

	bool VulkanGpuChunkRenderer::CreateFramebuffers()
	{
		framebuffers_.resize(swapchain_image_views_.size());
		for (std::size_t index = 0; index < swapchain_image_views_.size(); ++index)
		{
			std::array attachments{ swapchain_image_views_[index], depth_view_ };
			VkFramebufferCreateInfo framebuffer_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebuffer_info.renderPass = render_pass_;
			framebuffer_info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
			framebuffer_info.pAttachments = attachments.data();
			framebuffer_info.width = extent_.width;
			framebuffer_info.height = extent_.height;
			framebuffer_info.layers = 1u;
			if (vkCreateFramebuffer(device_, &framebuffer_info, nullptr, &framebuffers_[index]) != VK_SUCCESS) return false;
		}
		return true;
	}

	std::uint32_t VulkanGpuChunkRenderer::TextureLayer(ve::blocks::BlockId block, ve::blocks::BlockFace face) const noexcept
	{
		const std::size_t index = FaceIndex(block, face);
		if (index >= texture_layers_.size()) return 0u;
		return texture_layers_[index];
	}

	void VulkanGpuChunkRenderer::RebuildMesh(const ve::world::World& world, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const
	{
		const ve::world::WorldMetrics metrics = world.Metrics();
		const int width = metrics.worldSizeChunks * Chunk::CHUNK_WIDTH;
		const int height = Chunk::CHUNK_HEIGHT;
		const int depth = metrics.worldSizeChunks * Chunk::CHUNK_DEPTH;
		vertices.clear();
		indices.clear();
		vertices.reserve(140'000u);
		indices.reserve(210'000u);

		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				for (int z = 0; z < depth; ++z)
				{
					const ve::blocks::BlockId block = world.GetBlock(x, y, z);
					if (!IsRenderableBlock(block)) continue;
					for (const BlockFaceGeometry& face : kFaces)
					{
						const ve::blocks::BlockId neighbor = world.GetBlock(x + face.neighbor_offset.x, y + face.neighbor_offset.y, z + face.neighbor_offset.z);
						if (OccludesNeighborFaces(neighbor)) continue;
						const std::uint32_t base = static_cast<std::uint32_t>(vertices.size());
						const float layer = static_cast<float>(TextureLayer(block, face.face));
						const glm::vec3 origin{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
						for (std::size_t corner = 0; corner < face.corners.size(); ++corner)
						{
							const glm::vec3 position = origin + face.corners[corner];
							vertices.push_back(VoxelVertex{
								position.x,
								position.y,
								position.z,
								kFaceUvs[corner].x,
								kFaceUvs[corner].y,
								layer,
								face.light
							});
						}
						indices.insert(indices.end(), { base, base + 1u, base + 2u, base, base + 2u, base + 3u });
					}
				}
			}
		}
	}

	bool VulkanGpuChunkRenderer::UploadMeshBuffers(std::span<const VoxelVertex> vertices, std::span<const std::uint32_t> indices)
	{
		ReleaseMeshBuffers();
		index_count_ = static_cast<std::uint32_t>(indices.size());
		if (vertices.empty() || indices.empty()) return true;
		const VkDeviceSize vertex_bytes = static_cast<VkDeviceSize>(vertices.size_bytes());
		const VkDeviceSize index_bytes = static_cast<VkDeviceSize>(indices.size_bytes());
		return UploadDeviceLocalBuffer(vertices.data(), vertex_bytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertex_buffer_, vertex_memory_) &&
			UploadDeviceLocalBuffer(indices.data(), index_bytes, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, index_buffer_, index_memory_);
	}

	bool VulkanGpuChunkRenderer::EnsureWorldMesh(const ve::world::World& world)
	{
		if (mesh_valid_ && mesh_revision_ == world.Revision()) return true;
		std::vector<VoxelVertex> vertices;
		std::vector<std::uint32_t> indices;
		RebuildMesh(world, vertices, indices);
		if (!UploadMeshBuffers(vertices, indices)) return false;
		mesh_revision_ = world.Revision();
		mesh_valid_ = true;
		return true;
	}

	bool VulkanGpuChunkRenderer::Record(VkCommandBuffer command_buffer,
		std::uint32_t image_index,
		const Camera& camera,
		VulkanOverlayRecordCallback overlay_callback,
		void* overlay_user_data)
	{
		if (!initialized_ || image_index >= framebuffers_.size()) return false;
		VkClearValue clear_color{};
		clear_color.color = { { 0.50f, 0.66f, 0.84f, 1.0f } };
		VkClearValue clear_depth{};
		clear_depth.depthStencil = { 1.0f, 0u };
		std::array clear_values{ clear_color, clear_depth };

		VkRenderPassBeginInfo begin_info{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		begin_info.renderPass = render_pass_;
		begin_info.framebuffer = framebuffers_[image_index];
		begin_info.renderArea.extent = extent_;
		begin_info.clearValueCount = static_cast<std::uint32_t>(clear_values.size());
		begin_info.pClearValues = clear_values.data();
		vkCmdBeginRenderPass(command_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.width = static_cast<float>(extent_.width);
		viewport.height = static_cast<float>(extent_.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.extent = extent_;
		vkCmdSetViewport(command_buffer, 0u, 1u, &viewport);
		vkCmdSetScissor(command_buffer, 0u, 1u, &scissor);
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0u, 1u, &descriptor_set_, 0u, nullptr);

		const float aspect = static_cast<float>(extent_.width) / static_cast<float>(std::max(extent_.height, 1u));
		glm::mat4 projection = glm::perspective(glm::radians(72.0f), aspect, 0.05f, 512.0f);
		projection[1][1] *= -1.0f;
		const glm::mat4 mvp = projection * camera.GetViewMatrix();
		vkCmdPushConstants(command_buffer, pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT, 0u, sizeof(glm::mat4), &mvp);

		if (index_count_ > 0u)
		{
			const VkDeviceSize offset = 0u;
			vkCmdBindVertexBuffers(command_buffer, 0u, 1u, &vertex_buffer_, &offset);
			vkCmdBindIndexBuffer(command_buffer, index_buffer_, 0u, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(command_buffer, index_count_, 1u, 0u, 0, 0u);
		}
		if (overlay_callback != nullptr) overlay_callback(command_buffer, overlay_user_data);
		vkCmdEndRenderPass(command_buffer);
		return true;
	}

	void VulkanGpuChunkRenderer::ReleaseMeshBuffers()
	{
		if (vertex_buffer_ != VK_NULL_HANDLE) vkDestroyBuffer(device_, vertex_buffer_, nullptr);
		if (vertex_memory_ != VK_NULL_HANDLE) vkFreeMemory(device_, vertex_memory_, nullptr);
		if (index_buffer_ != VK_NULL_HANDLE) vkDestroyBuffer(device_, index_buffer_, nullptr);
		if (index_memory_ != VK_NULL_HANDLE) vkFreeMemory(device_, index_memory_, nullptr);
		vertex_buffer_ = VK_NULL_HANDLE;
		vertex_memory_ = VK_NULL_HANDLE;
		index_buffer_ = VK_NULL_HANDLE;
		index_memory_ = VK_NULL_HANDLE;
		index_count_ = 0u;
		mesh_valid_ = false;
		mesh_revision_ = 0u;
	}

	void VulkanGpuChunkRenderer::ReleaseSwapchainResources()
	{
		for (VkFramebuffer framebuffer : framebuffers_)
		{
			if (framebuffer != VK_NULL_HANDLE) vkDestroyFramebuffer(device_, framebuffer, nullptr);
		}
		framebuffers_.clear();
		if (depth_view_ != VK_NULL_HANDLE) vkDestroyImageView(device_, depth_view_, nullptr);
		if (depth_image_ != VK_NULL_HANDLE) vkDestroyImage(device_, depth_image_, nullptr);
		if (depth_memory_ != VK_NULL_HANDLE) vkFreeMemory(device_, depth_memory_, nullptr);
		depth_view_ = VK_NULL_HANDLE;
		depth_image_ = VK_NULL_HANDLE;
		depth_memory_ = VK_NULL_HANDLE;
		for (VkImageView image_view : swapchain_image_views_)
		{
			if (image_view != VK_NULL_HANDLE) vkDestroyImageView(device_, image_view, nullptr);
		}
		swapchain_image_views_.clear();
		extent_ = {};
	}

	void VulkanGpuChunkRenderer::ReleaseTextureResources()
	{
		if (texture_sampler_ != VK_NULL_HANDLE) vkDestroySampler(device_, texture_sampler_, nullptr);
		if (texture_view_ != VK_NULL_HANDLE) vkDestroyImageView(device_, texture_view_, nullptr);
		if (texture_image_ != VK_NULL_HANDLE) vkDestroyImage(device_, texture_image_, nullptr);
		if (texture_memory_ != VK_NULL_HANDLE) vkFreeMemory(device_, texture_memory_, nullptr);
		texture_sampler_ = VK_NULL_HANDLE;
		texture_view_ = VK_NULL_HANDLE;
		texture_image_ = VK_NULL_HANDLE;
		texture_memory_ = VK_NULL_HANDLE;
	}

	void VulkanGpuChunkRenderer::ReleasePipelineResources()
	{
		if (pipeline_ != VK_NULL_HANDLE) vkDestroyPipeline(device_, pipeline_, nullptr);
		if (pipeline_layout_ != VK_NULL_HANDLE) vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
		if (render_pass_ != VK_NULL_HANDLE) vkDestroyRenderPass(device_, render_pass_, nullptr);
		if (descriptor_pool_ != VK_NULL_HANDLE) vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
		if (descriptor_set_layout_ != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);
		pipeline_ = VK_NULL_HANDLE;
		pipeline_layout_ = VK_NULL_HANDLE;
		render_pass_ = VK_NULL_HANDLE;
		descriptor_pool_ = VK_NULL_HANDLE;
		descriptor_set_layout_ = VK_NULL_HANDLE;
		descriptor_set_ = VK_NULL_HANDLE;
	}

	void VulkanGpuChunkRenderer::Release()
	{
		if (device_ != VK_NULL_HANDLE) vkDeviceWaitIdle(device_);
		ReleaseMeshBuffers();
		ReleaseSwapchainResources();
		ReleaseTextureResources();
		ReleasePipelineResources();
		backend_ = nullptr;
		device_ = VK_NULL_HANDLE;
		physical_device_ = VK_NULL_HANDLE;
		command_pool_ = VK_NULL_HANDLE;
		color_format_ = VK_FORMAT_UNDEFINED;
		texture_layers_.fill(0u);
		initialized_ = false;
	}
}
