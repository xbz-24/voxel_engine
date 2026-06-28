#pragma once

#include <string>
#include <vector>

namespace voxel
{
	// TODO: Add asset lifetime policies for embedded data, filesystem paths, hot reload, and packaged archives.
	struct TextureAsset
	{
		std::string name;
		std::string path;
	};

	struct ModelAsset
	{
		std::string name;
		std::string path;
	};

	struct SoundAsset
	{
		std::string name;
		std::string path;
	};

	struct AssetCatalog
	{
		std::vector<TextureAsset> textures;
		std::vector<ModelAsset> models;
		std::vector<SoundAsset> sounds;

		AssetCatalog& Texture(std::string name, std::string path);
		AssetCatalog& Model(std::string name, std::string path);
		AssetCatalog& Sound(std::string name, std::string path);
		[[nodiscard]] std::vector<std::string> Validate(bool require_existing_files = false) const;
	};
}
