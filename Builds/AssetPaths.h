#pragma once

#include <filesystem>

namespace ve::assets
{
	/**
	 * Stores filesystem paths used by the engine runtime.
	 */
	struct AssetPaths
	{
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
	};

	/**
	 * Builds asset paths relative to the application source file.
	 *
	 * @param applicationSourceFile Path to a source file inside the Builds directory.
	 * @return Resolved asset path bundle.
	 */
	AssetPaths ResolveFromSourceFile(const std::filesystem::path& applicationSourceFile);
}
