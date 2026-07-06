#include <doctest/doctest.h>

#include "AssetPaths.h"
#include "voxel/Assets.h"

#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("asset paths prefer configured search roots")
{
	const std::filesystem::path root_directory = std::filesystem::temp_directory_path() /
		("voxel_asset_root_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
	std::filesystem::create_directories(root_directory / "assets/textures/block");

	ve::assets::AssetPathResolveOptions options;
	options.search_roots.push_back(root_directory);
	const ve::assets::AssetPaths paths = ve::assets::Resolve(options);

	CHECK(paths.rootDirectory == root_directory.lexically_normal());
	CHECK(paths.blockTexturesDirectory == paths.rootDirectory / "assets/textures/block/");
	CHECK(paths.crosshairTexture == paths.rootDirectory / "assets/textures/gui/sprites/hud/crosshair.png");

	std::error_code cleanup_error;
	std::filesystem::remove_all(root_directory, cleanup_error);
}

TEST_CASE("asset paths accept an explicit assets directory")
{
	const std::filesystem::path root_directory = std::filesystem::temp_directory_path() /
		("voxel_asset_folder_root_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
	const std::filesystem::path assets_directory = root_directory / "assets";
	std::filesystem::create_directories(assets_directory / "textures/block");

	ve::assets::AssetPathResolveOptions options;
	options.search_roots.push_back(assets_directory);
	const ve::assets::AssetPaths paths = ve::assets::Resolve(options);

	CHECK(paths.rootDirectory == root_directory.lexically_normal());
	CHECK(paths.blockTexturesDirectory == root_directory / "assets/textures/block/");

	std::error_code cleanup_error;
	std::filesystem::remove_all(root_directory, cleanup_error);
}

TEST_CASE("asset paths accept public asset catalog search roots")
{
	const std::filesystem::path root_directory = std::filesystem::temp_directory_path() /
		("voxel_asset_catalog_root_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
	const std::filesystem::path assets_directory = root_directory / "assets";
	std::filesystem::create_directories(assets_directory / "textures/block");

	voxel::AssetCatalog catalog{};
	catalog.SearchRoot(assets_directory.string());
	ve::assets::AssetPathResolveOptions options;
	options.AddSearchRoots(catalog.search_roots);
	const ve::assets::AssetPaths paths = ve::assets::Resolve(options);

	CHECK(paths.rootDirectory == root_directory.lexically_normal());
	CHECK(paths.blockTexturesDirectory == root_directory / "assets/textures/block/");

	std::error_code cleanup_error;
	std::filesystem::remove_all(root_directory, cleanup_error);
}

TEST_CASE("asset paths discover an asset root without explicit search roots")
{
	const ve::assets::AssetPaths paths = ve::assets::Resolve();

	CHECK(std::filesystem::is_directory(paths.rootDirectory / "assets"));
}
