#pragma once

#include "Block.h"

#include <volk.h>

#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include <glm/glm.hpp>

namespace ve::rendering
{
	struct VulkanRasterFrameWorldSnapshot
	{
		std::vector<ve::blocks::BlockId> blocks;
		int width = 0;
		int height = 0;
		int depth = 0;
		std::uint64_t world_revision = 0;

		[[nodiscard]] ve::blocks::BlockId GetBlock(const glm::ivec3& position) const noexcept;
	};

	struct VulkanRasterCachedSampleRay
	{
		std::uint32_t x = 0;
		std::uint32_t y = 0;
		std::uint32_t x_end = 0;
		std::uint32_t y_end = 0;
		glm::vec3 direction{ 0.0f, 0.0f, -1.0f };
	};

	struct VulkanRasterCpuTexture
	{
		std::vector<std::uint32_t> pixels;
		std::uint32_t width = 1;
		std::uint32_t height = 1;

		[[nodiscard]] std::uint32_t Sample(float u, float v) const noexcept;
	};

	struct VulkanRasterTextureLibrary
	{
		std::vector<VulkanRasterCpuTexture> textures;
		std::array<std::uint16_t, static_cast<std::size_t>(ve::blocks::BlockId::Count) * static_cast<std::size_t>(ve::blocks::BlockFace::Count)> face_texture_indices{};
		bool loaded = false;
	};

	struct VulkanRasterWork
	{
		const VulkanRasterFrameWorldSnapshot* world = nullptr;
		glm::vec3 origin{ 0.0f };
		VkFormat format = VK_FORMAT_UNDEFINED;
		float max_ray_distance = 128.0f;
		float fog_strength = 0.45f;
	};

	struct VulkanRasterUpscaleRange
	{
		std::uint32_t begin = 0;
		std::uint32_t end = 0;
	};
}
