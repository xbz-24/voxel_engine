#pragma once

#include <filesystem>
#include <vector>

namespace ve::assets
{
	/**
	 * Stores filesystem paths used by the engine runtime.
	 */
	struct AssetPaths
	{
		// TODO: Merge this with public AssetCatalog so built-in and user-provided assets resolve through one pipeline.
		std::filesystem::path rootDirectory;
		std::filesystem::path blockTexturesDirectory;
		std::filesystem::path environmentTexturesDirectory;
		std::filesystem::path crosshairTexture;
		std::filesystem::path hotbarTexture;
		std::filesystem::path hotbarSelectionTexture;
		std::filesystem::path experienceBarTexture;
		std::filesystem::path healthTexture;
		std::filesystem::path hungerTexture;
		std::filesystem::path fontTexture;
		std::filesystem::path menuBackgroundTexture;
		std::filesystem::path menuButtonTexture;
		std::filesystem::path menuButtonHighlightedTexture;
		std::filesystem::path menuSliderTexture;
		std::filesystem::path menuSliderHandleTexture;
	};

	struct AssetPathResolveOptions
	{
		std::vector<std::filesystem::path> search_roots;
	};

	/**
	 * Builds asset paths relative to the first configured or discovered asset root.
	 *
	 * @return Resolved asset path bundle.
	 */
	AssetPaths Resolve(const AssetPathResolveOptions& options = {});
}
