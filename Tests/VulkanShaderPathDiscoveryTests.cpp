#include <doctest/doctest.h>

#include "VulkanShaderPathDiscovery.h"

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>

namespace
{
	constexpr std::array<std::string_view, 5> ShaderFiles{{
		"voxel_chunk.vert.spv", "voxel_chunk.frag.spv",
		"voxel_sky.vert.spv", "voxel_sky.frag.spv", "voxel_shadow.vert.spv"
	}};

	class TemporaryDirectory
	{
	public:
		TemporaryDirectory()
			: path(std::filesystem::temp_directory_path() /
				("voxel_shader_paths_" + std::to_string(
					std::chrono::steady_clock::now().time_since_epoch().count())))
		{
			std::filesystem::create_directories(path);
		}
		~TemporaryDirectory()
		{
			std::error_code error;
			std::filesystem::remove_all(path, error);
		}
		std::filesystem::path path;
	};

	void WriteShader(const std::filesystem::path& directory, std::string_view name,
		std::size_t byte_count = 20u, bool valid_magic = true)
	{
		std::filesystem::create_directories(directory);
		std::ofstream output(directory / name, std::ios::binary);
		constexpr std::array<unsigned char, 4> SpirvMagic{{ 0x03u, 0x02u, 0x23u, 0x07u }};
		for (std::size_t index = 0; index < byte_count; ++index)
			output.put(static_cast<char>(valid_magic && index < SpirvMagic.size()
				? SpirvMagic[index] : 0u));
	}

	void WriteBundle(const std::filesystem::path& directory)
	{
		for (const std::string_view name : ShaderFiles) WriteShader(directory, name);
	}
}

TEST_CASE("explicit complete Vulkan shader directory wins")
{
	TemporaryDirectory temporary;
	const std::filesystem::path shader_directory = temporary.path / "explicit";
	WriteBundle(shader_directory);

	const std::filesystem::path resolved = ve::assets::ResolveVulkanShaderDirectory(
		shader_directory, temporary.path / "unrelated");

	CHECK(resolved == std::filesystem::absolute(shader_directory).lexically_normal());
}

TEST_CASE("explicit invalid Vulkan shader directory does not fall back")
{
	TemporaryDirectory temporary;
	const std::filesystem::path invalid_directory = temporary.path / "invalid";
	WriteBundle(temporary.path / "shaders/vulkan");
	for (std::size_t index = 0; index + 1u < ShaderFiles.size(); ++index)
		WriteShader(invalid_directory, ShaderFiles[index]);

	CHECK(ve::assets::ResolveVulkanShaderDirectory(
		invalid_directory, temporary.path).empty());
	WriteShader(invalid_directory, ShaderFiles.back(), 3u);
	CHECK_FALSE(ve::assets::IsCompleteVulkanShaderDirectory(invalid_directory));
	WriteShader(invalid_directory, ShaderFiles.back(), 20u, false);
	CHECK_FALSE(ve::assets::IsCompleteVulkanShaderDirectory(invalid_directory));
}

TEST_CASE("Vulkan shader discovery skips partial bundles and searches ancestors")
{
	TemporaryDirectory temporary;
	const std::filesystem::path complete = temporary.path / "shaders/vulkan";
	const std::filesystem::path partial = temporary.path / "nested/generated/shaders/vulkan";
	WriteBundle(complete);
	WriteShader(partial, ShaderFiles.front());

	const std::filesystem::path resolved = ve::assets::ResolveVulkanShaderDirectory(
		std::nullopt, temporary.path / "nested/app/bin");

	CHECK(resolved == std::filesystem::absolute(complete).lexically_normal());
}
