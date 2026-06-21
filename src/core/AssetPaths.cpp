#include "AssetPaths.h"

namespace ve::assets
{
	AssetPaths Resolve()
	{
		AssetPaths paths;
		paths.rootDirectory = std::filesystem::path{ ROOT_DIR };
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
