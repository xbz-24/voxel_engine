#include "VulkanSoftwareVoxelRasterizer.h"

#include "BlockDefinitions.h"
#include "Logger.h"
#include "VulkanSoftwareRasterizerColor.h"

#include <boost/container/flat_map.hpp>
#include <stb_image.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <string>

namespace ve::rendering
{
	namespace
	{
		std::size_t FaceIndex(ve::blocks::BlockId block, ve::blocks::BlockFace face) noexcept
		{
			return (static_cast<std::size_t>(block) * static_cast<std::size_t>(ve::blocks::BlockFace::Count)) +
				static_cast<std::size_t>(face);
		}
	}
	void VulkanSoftwareVoxelRasterizer::LoadBlockTextures(const std::filesystem::path& block_texture_directory)
	{
		texture_library_ = {};
		texture_library_.textures.push_back(CpuTexture{ std::vector<std::uint32_t>{ PackRgb({ 132, 132, 132 }) }, 1u, 1u });

		boost::container::flat_map<std::string, std::uint16_t> texture_indices;
		const auto load_texture = [&](const char* file_name, Rgb fallback) -> std::uint16_t
		{
			if (file_name == nullptr) return 0u;
			const std::string key{ file_name };
			if (const auto existing = texture_indices.find(key); existing != texture_indices.end()) return existing->second;

			CpuTexture texture;
			const std::filesystem::path texture_path = block_texture_directory / key;
			int width = 0;
			int height = 0;
			int channels = 0;
			unsigned char* data = stbi_load(texture_path.string().c_str(), &width, &height, &channels, 4);
			if (data != nullptr && width > 0 && height > 0)
			{
				texture.width = static_cast<std::uint32_t>(width);
				texture.height = static_cast<std::uint32_t>(height);
				texture.pixels.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
				for (std::size_t index = 0; index < texture.pixels.size(); ++index)
				{
					const unsigned char* pixel = data + (index * 4u);
					const Rgb color{ pixel[0], pixel[1], pixel[2] };
					texture.pixels[index] = pixel[3] < 16u ? PackRgb(fallback) : PackRgb(color);
				}
			}
			else
			{
				texture = CpuTexture{ std::vector<std::uint32_t>{ PackRgb(fallback) }, 1u, 1u };
			}
			stbi_image_free(data);

			const auto next_index = static_cast<std::uint16_t>(texture_library_.textures.size());
			texture_library_.textures.push_back(std::move(texture));
			texture_indices.emplace(key, next_index);
			return next_index;
		};

		for (const ve::blocks::BlockDefinition& definition : ve::blocks::BuiltInBlockDefinitions)
		{
			const Rgb fallback = BaseBlockColor(definition.id);
			const std::array<const char*, static_cast<std::size_t>(ve::blocks::BlockFace::Count)> files{
				definition.textures.top,
				definition.textures.bottom,
				definition.textures.front,
				definition.textures.back,
				definition.textures.left,
				definition.textures.right
			};
			for (std::size_t face = 0; face < files.size(); ++face)
			{
				texture_library_.face_texture_indices[FaceIndex(definition.id, static_cast<ve::blocks::BlockFace>(face))] = load_texture(files[face], fallback);
			}
		}
		texture_library_.loaded = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Loaded Vulkan demo block textures: " + std::to_string(texture_library_.textures.size() - 1u));
	}
	const VulkanSoftwareVoxelRasterizer::CpuTexture& VulkanSoftwareVoxelRasterizer::TextureFor(ve::blocks::BlockId block, ve::blocks::BlockFace face) const noexcept
	{
		static const CpuTexture fallback{ std::vector<std::uint32_t>{ PackRgb({ 132, 132, 132 }) }, 1u, 1u };
		if (!texture_library_.loaded || texture_library_.textures.empty()) return fallback;
		const std::size_t block_index = static_cast<std::size_t>(block);
		if (block_index >= static_cast<std::size_t>(ve::blocks::BlockId::Count)) return texture_library_.textures.front();
		const std::uint16_t texture_index = texture_library_.face_texture_indices[FaceIndex(block, face)];
		if (texture_index >= texture_library_.textures.size()) return texture_library_.textures.front();
		return texture_library_.textures[texture_index];
	}
}
