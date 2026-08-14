#include "EngineConfigValidationInternal.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace voxel::detail::config_validation
{
	namespace
	{
		constexpr std::uint32_t SpirvMagic = 0x07230203u;
		constexpr std::uintmax_t SpirvHeaderBytes = 5u * sizeof(std::uint32_t);
		constexpr std::array<std::string_view, 5> RequiredVulkanShaders{{
			"voxel_chunk.vert.spv", "voxel_chunk.frag.spv", "voxel_sky.vert.spv",
			"voxel_sky.frag.spv", "voxel_shadow.vert.spv"
		}};

		void ValidateShaderFile(const std::filesystem::path& shader_file,
			std::vector<std::string>& issues)
		{
			std::error_code error;
			if (!std::filesystem::is_regular_file(shader_file, error))
			{
				issues.push_back(shader_file.filename().string() + " must be a regular shader file");
				return;
			}
			const std::uintmax_t size = std::filesystem::file_size(shader_file, error);
			std::uint32_t magic = 0u;
			std::ifstream input{ shader_file, std::ios::binary };
			input.read(reinterpret_cast<char*>(&magic), sizeof(magic));
			if (error || size < SpirvHeaderBytes || size % sizeof(std::uint32_t) != 0u ||
				!input.good() || magic != SpirvMagic)
				issues.push_back(shader_file.filename().string() +
					" must contain a SPIR-V header and whole 32-bit words");
		}

		void ValidateShaderDirectory(const std::string& directory,
			std::vector<std::string>& issues)
		{
			std::error_code error;
			const std::filesystem::path path{ directory };
			if (!std::filesystem::is_directory(path, error))
			{
				issues.push_back("runtime_layout.vulkan_shader_directory must name an existing directory");
				return;
			}
			for (const std::string_view shader : RequiredVulkanShaders)
				ValidateShaderFile(path / shader, issues);
		}
	}

	void ValidateRuntimeLayout(const EngineConfig& config, std::vector<std::string>& issues)
	{
		if (!config.runtime_layout) return;
		const RuntimeLayout& layout = *config.runtime_layout;
		std::error_code error;
		if (layout.asset_directory.empty())
			issues.push_back("runtime_layout.asset_directory must not be empty");
		else if (!std::filesystem::is_directory(layout.asset_directory, error))
			issues.push_back("runtime_layout.asset_directory must name an existing directory");
		if (layout.vulkan_shader_directory.empty())
		{
			if (config.graphics_api == GraphicsApi::Vulkan)
				issues.push_back("runtime_layout.vulkan_shader_directory is required for Vulkan");
			return;
		}
		ValidateShaderDirectory(layout.vulkan_shader_directory, issues);
	}
}
