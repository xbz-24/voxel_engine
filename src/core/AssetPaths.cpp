#include "AssetPaths.h"

#include <array>
#include <cstddef>
#include <system_error>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace ve::assets
{
	namespace
	{
		[[nodiscard]] bool HasAssetDirectory(const std::filesystem::path& root_directory)
		{
			std::error_code error;
			return std::filesystem::is_directory(root_directory / "assets", error);
		}

		[[nodiscard]] std::filesystem::path NormalizeSearchRoot(const std::filesystem::path& search_root)
		{
			std::error_code error;
			const std::filesystem::path absolute_root = std::filesystem::absolute(search_root, error);
			const std::filesystem::path normalized_root = error ? search_root : absolute_root;
			return normalized_root.lexically_normal();
		}

		[[nodiscard]] std::filesystem::path FindAncestorWithAssets(std::filesystem::path start_directory)
		{
			if (start_directory.empty()) return {};
			start_directory = NormalizeSearchRoot(start_directory);
			for (std::filesystem::path current_directory = start_directory; !current_directory.empty();)
			{
				if (HasAssetDirectory(current_directory)) return current_directory;
				const std::filesystem::path parent_directory = current_directory.parent_path();
				if (parent_directory == current_directory) break;
				current_directory = parent_directory;
			}
			return {};
		}

		[[nodiscard]] std::filesystem::path ExecutableDirectory()
		{
#if defined(_WIN32)
			std::array<wchar_t, 32768> executable_path_buffer{};
			const DWORD copied_character_count = GetModuleFileNameW(
				nullptr,
				executable_path_buffer.data(),
				static_cast<DWORD>(executable_path_buffer.size()));
			if (copied_character_count > 0 &&
				static_cast<std::size_t>(copied_character_count) < executable_path_buffer.size())
			{
				return std::filesystem::path{
					std::wstring{ executable_path_buffer.data(), copied_character_count }
				}.parent_path();
			}
#endif
			return std::filesystem::current_path();
		}

		[[nodiscard]] std::filesystem::path DevelopmentRootDirectory()
		{
#if defined(ROOT_DIR)
			return std::filesystem::path{ ROOT_DIR };
#else
			return {};
#endif
		}

		[[nodiscard]] std::filesystem::path ResolveRootDirectory(const AssetPathResolveOptions& options)
		{
			for (const std::filesystem::path& search_root : options.search_roots)
			{
				if (!search_root.empty()) return NormalizeSearchRoot(search_root);
			}

			const std::filesystem::path executable_asset_root = FindAncestorWithAssets(ExecutableDirectory());
			if (!executable_asset_root.empty()) return executable_asset_root;

			const std::filesystem::path development_root = DevelopmentRootDirectory();
			if (!development_root.empty()) return NormalizeSearchRoot(development_root);

			return NormalizeSearchRoot(std::filesystem::current_path());
		}
	}

	AssetPaths Resolve(const AssetPathResolveOptions& options)
	{
		AssetPaths paths;
		paths.rootDirectory = ResolveRootDirectory(options);
		paths.blockTexturesDirectory = paths.rootDirectory / "assets/textures/block/";
		paths.environmentTexturesDirectory = paths.rootDirectory / "assets/textures/environment/";
		paths.crosshairTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/crosshair.png";
		paths.hotbarTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/hotbar.png";
		paths.hotbarSelectionTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/hotbar_selection.png";
		paths.experienceBarTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/experience_bar_background.png";
		paths.healthTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/heart/full.png";
		paths.hungerTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/food_full.png";
		paths.fontTexture = paths.rootDirectory / "assets/textures/font/ascii.png";
		paths.menuBackgroundTexture = paths.rootDirectory / "assets/textures/gui/menu_background.png";
		paths.menuButtonTexture = paths.rootDirectory / "assets/textures/gui/sprites/widget/button.png";
		paths.menuButtonHighlightedTexture = paths.rootDirectory / "assets/textures/gui/sprites/widget/button_highlighted.png";
		paths.menuSliderTexture = paths.rootDirectory / "assets/textures/gui/sprites/widget/slider.png";
		paths.menuSliderHandleTexture = paths.rootDirectory / "assets/textures/gui/sprites/widget/slider_handle.png";
		return paths;
	}
}
