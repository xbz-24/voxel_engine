#pragma once

#include <filesystem>

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

	/**
	 * Builds asset paths relative to the configured project root.
	 *
	 * @return Resolved asset path bundle.
	 */
	// TODO: Stop relying on ROOT_DIR for packaged builds; prefer executable-relative and user-specified search roots.
	AssetPaths Resolve();
}
