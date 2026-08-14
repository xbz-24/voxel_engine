#include "VulkanShaderPathDiscovery.h"

#include "ExecutablePath.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <string_view>
#include <system_error>

namespace ve::assets
{
	namespace
	{
		constexpr std::uint32_t SpirvMagic = 0x07230203u;
		constexpr std::uintmax_t SpirvHeaderBytes = 5u * sizeof(std::uint32_t);
		constexpr std::array<std::string_view, 5> RequiredShaderFiles{{
			"voxel_chunk.vert.spv", "voxel_chunk.frag.spv",
			"voxel_sky.vert.spv", "voxel_sky.frag.spv",
			"voxel_shadow.vert.spv"
		}};

		[[nodiscard]] std::filesystem::path NormalizePath(const std::filesystem::path& path)
		{
			std::error_code error;
			const std::filesystem::path absolute_path = std::filesystem::absolute(path, error);
			return (error ? path : absolute_path).lexically_normal();
		}

		[[nodiscard]] bool IsUsableShaderFile(const std::filesystem::path& path)
		{
			std::error_code error;
			if (!std::filesystem::is_regular_file(path, error) || error) return false;
			const std::uintmax_t byte_count = std::filesystem::file_size(path, error);
			if (error || byte_count < SpirvHeaderBytes ||
				byte_count % sizeof(std::uint32_t) != 0u) return false;
			std::uint32_t magic = 0u;
			std::ifstream input{ path, std::ios::binary };
			input.read(reinterpret_cast<char*>(&magic), sizeof(magic));
			return input.good() && magic == SpirvMagic;
		}

		[[nodiscard]] std::filesystem::path FindAbove(std::filesystem::path start_directory)
		{
			for (std::filesystem::path current = NormalizePath(start_directory); !current.empty();)
			{
				for (const std::filesystem::path& relative : {
					std::filesystem::path{ "shaders/vulkan" },
					std::filesystem::path{ "generated/shaders/vulkan" } })
				{
					const std::filesystem::path candidate = current / relative;
					if (IsCompleteVulkanShaderDirectory(candidate)) return candidate;
				}
				const std::filesystem::path parent = current.parent_path();
				if (parent == current) break;
				current = parent;
			}
			return {};
		}
	}

	bool IsCompleteVulkanShaderDirectory(const std::filesystem::path& shader_directory)
	{
		if (shader_directory.empty()) return false;
		for (const std::string_view file_name : RequiredShaderFiles)
		{
			if (!IsUsableShaderFile(shader_directory / file_name)) return false;
		}
		return true;
	}

	std::filesystem::path ResolveVulkanShaderDirectory(
		const std::optional<std::filesystem::path>& explicit_directory,
		const std::filesystem::path& discovery_start_directory)
	{
		if (explicit_directory.has_value())
		{
			if (explicit_directory->empty()) return {};
			const std::filesystem::path normalized = NormalizePath(*explicit_directory);
			return IsCompleteVulkanShaderDirectory(normalized) ? normalized : std::filesystem::path{};
		}
		const std::filesystem::path start = discovery_start_directory.empty()
			? ve::core::ExecutableDirectory() : discovery_start_directory;
		return FindAbove(start);
	}
}
