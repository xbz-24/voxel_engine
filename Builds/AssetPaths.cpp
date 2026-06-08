#include "AssetPaths.h"

namespace ve::assets
{
	AssetPaths ResolveFromSourceFile(const std::filesystem::path& applicationSourceFile)
	{
		AssetPaths paths;
		paths.rootDirectory = applicationSourceFile.parent_path().parent_path();
		paths.blockTexturesDirectory = paths.rootDirectory / "assets/textures/block/";
		paths.environmentTexturesDirectory = paths.rootDirectory / "assets/textures/environment/";
		paths.crosshairTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/crosshair.png";
		paths.hotbarTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/hotbar.png";
		paths.hotbarSelectionTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/hotbar_selection.png";
		paths.experienceBarTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/experience_bar_background.png";
		paths.healthTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/heart/full.png";
		paths.hungerTexture = paths.rootDirectory / "assets/textures/gui/sprites/hud/food_full.png";
		paths.fontTexture = paths.rootDirectory / "assets/textures/font/ascii.png";
		return paths;
	}
}
