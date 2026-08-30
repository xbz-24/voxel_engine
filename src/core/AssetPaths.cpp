#include "AssetPaths.h"
#include "AssetRootDiscovery.h"
#include "VulkanShaderPathDiscovery.h"

namespace ve::assets
{
	namespace
	{
		[[nodiscard]] std::filesystem::path TexturePath(const AssetPaths& paths, const char* relative_path)
		{
			return paths.assetsDirectory / "textures" / relative_path;
		}
	}

	AssetPaths Resolve(const AssetPathResolveOptions& options)
	{
		AssetPaths paths;
		paths.assetsDirectory = detail::ResolveAssetDirectory(options);
		paths.rootDirectory = paths.assetsDirectory.parent_path();
		paths.vulkanShaderDirectory = ResolveVulkanShaderDirectory(
			options.explicit_vulkan_shader_directory);
		paths.blockTexturesDirectory = TexturePath(paths, "block/");
		paths.environmentTexturesDirectory = TexturePath(paths, "environment/");
		paths.crosshairTexture = TexturePath(paths, "gui/sprites/hud/crosshair.png");
		paths.hotbarTexture = TexturePath(paths, "gui/sprites/hud/hotbar.png");
		paths.hotbarSelectionTexture = TexturePath(paths, "gui/sprites/hud/hotbar_selection.png");
		paths.experienceBarTexture = TexturePath(paths, "gui/sprites/hud/experience_bar_background.png");
		paths.healthTexture = TexturePath(paths, "gui/sprites/hud/heart/full.png");
		paths.hungerTexture = TexturePath(paths, "gui/sprites/hud/food_full.png");
		paths.fontTexture = TexturePath(paths, "font/ascii.png");
		paths.menuBackgroundTexture = TexturePath(paths, "gui/menu_background.png");
		paths.menuButtonTexture = TexturePath(paths, "gui/sprites/widget/button.png");
		paths.menuButtonHighlightedTexture = TexturePath(paths, "gui/sprites/widget/button_highlighted.png");
		paths.menuSliderTexture = TexturePath(paths, "gui/sprites/widget/slider.png");
		paths.menuSliderHandleTexture = TexturePath(paths, "gui/sprites/widget/slider_handle.png");
		return paths;
	}
}
