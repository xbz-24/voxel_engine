#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>

namespace ve::tests
{
	inline constexpr std::array<std::string_view, 5> RuntimeShaderFileNames{{
		"voxel_chunk.vert.spv", "voxel_chunk.frag.spv", "voxel_sky.vert.spv",
		"voxel_sky.frag.spv", "voxel_shadow.vert.spv"
	}};

	class TemporaryRuntimeLayout
	{
	public:
		explicit TemporaryRuntimeLayout(std::string_view label)
		{
			const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
			root_ = std::filesystem::temp_directory_path() /
				("voxel_engine_" + std::string{ label } + "_" + std::to_string(nonce));
			asset_directory_ = root_ / "assets";
			shader_directory_ = root_ / "shaders";
			std::filesystem::create_directories(asset_directory_);
			std::filesystem::create_directories(shader_directory_);
		}

		~TemporaryRuntimeLayout()
		{
			std::error_code error;
			std::filesystem::remove_all(root_, error);
		}

		TemporaryRuntimeLayout(const TemporaryRuntimeLayout&) = delete;
		TemporaryRuntimeLayout& operator=(const TemporaryRuntimeLayout&) = delete;

		[[nodiscard]] const std::filesystem::path& Assets() const noexcept { return asset_directory_; }
		[[nodiscard]] const std::filesystem::path& Shaders() const noexcept { return shader_directory_; }

		void WriteShader(std::string_view file_name, std::size_t byte_count = 20,
			bool valid_magic = true) const
		{
			std::ofstream output{ shader_directory_ / file_name, std::ios::binary };
			constexpr std::array<unsigned char, 4> SpirvMagic{{ 0x03u, 0x02u, 0x23u, 0x07u }};
			for (std::size_t byte = 0; byte < byte_count; ++byte)
				output.put(static_cast<char>(valid_magic && byte < SpirvMagic.size()
					? SpirvMagic[byte] : 0u));
		}

		void WriteValidShaderBundle() const
		{
			for (const std::string_view file_name : RuntimeShaderFileNames) WriteShader(file_name);
		}

	private:
		std::filesystem::path root_;
		std::filesystem::path asset_directory_;
		std::filesystem::path shader_directory_;
	};
}
