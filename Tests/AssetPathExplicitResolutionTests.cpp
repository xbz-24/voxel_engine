#include <doctest/doctest.h>

#include "AssetPaths.h"

#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

namespace
{
	class TemporaryDirectory
	{
	public:
		TemporaryDirectory()
			: path(std::filesystem::temp_directory_path() /
				("voxel_explicit_assets_" + std::to_string(
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
}

TEST_CASE("explicit asset directory is exact and wins over legacy search roots")
{
	TemporaryDirectory temporary;
	const std::filesystem::path asset_directory = temporary.path / "licensed-content";
	const std::filesystem::path legacy_root = temporary.path / "legacy";
	std::filesystem::create_directories(asset_directory / "textures/block");
	std::filesystem::create_directories(legacy_root / "assets/textures/block");

	ve::assets::AssetPathResolveOptions options;
	options.explicit_asset_directory = asset_directory;
	options.search_roots.push_back(legacy_root);
	const ve::assets::AssetPaths paths = ve::assets::Resolve(options);

	CHECK(paths.assetsDirectory == std::filesystem::absolute(asset_directory).lexically_normal());
	CHECK(paths.rootDirectory == paths.assetsDirectory.parent_path());
	CHECK(paths.blockTexturesDirectory == paths.assetsDirectory / "textures/block/");
}

TEST_CASE("empty explicit asset directory does not fall back to repository discovery")
{
	ve::assets::AssetPathResolveOptions options;
	options.explicit_asset_directory = std::filesystem::path{};
	const ve::assets::AssetPaths paths = ve::assets::Resolve(options);

	CHECK(paths.assetsDirectory.empty());
	CHECK(paths.rootDirectory.empty());
}
