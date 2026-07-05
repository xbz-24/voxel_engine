#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace voxel
{
	enum class AssetStorage
	{
		FilePath,
		EmbeddedData,
		PackagedArchive
	};

	struct AssetSource
	{
		AssetStorage storage = AssetStorage::FilePath;
		bool hot_reload = false;
		std::string location;
		std::string archive_path;
		std::vector<std::uint8_t> embedded_data;

		[[nodiscard]] static AssetSource File(std::string path);
		[[nodiscard]] static AssetSource Embedded(std::vector<std::uint8_t> bytes);
		[[nodiscard]] static AssetSource Archive(std::string archive_path, std::string entry_path);

		AssetSource& EnableHotReload(bool enabled = true) noexcept;
	};

	struct TextureAsset
	{
		std::string name;
		std::string path;
		AssetSource source{};
	};

	struct ModelAsset
	{
		std::string name;
		std::string path;
		AssetSource source{};
	};

	struct SoundAsset
	{
		std::string name;
		std::string path;
		AssetSource source{};
	};

	struct AssetCatalog
	{
		std::vector<std::string> search_roots;
		std::vector<TextureAsset> textures;
		std::vector<ModelAsset> models;
		std::vector<SoundAsset> sounds;

		AssetCatalog& SearchRoot(std::string path);
		AssetCatalog& Texture(std::string name, std::string path);
		AssetCatalog& Texture(std::string name, AssetSource source);
		AssetCatalog& Model(std::string name, std::string path);
		AssetCatalog& Model(std::string name, AssetSource source);
		AssetCatalog& Sound(std::string name, std::string path);
		AssetCatalog& Sound(std::string name, AssetSource source);
		[[nodiscard]] std::vector<std::string> Validate(bool require_existing_files = false) const;
	};
}
