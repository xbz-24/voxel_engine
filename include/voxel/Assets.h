#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace voxel
{
	/** @addtogroup voxel_sdk_assets
	 * @{
	 */
	/** Storage strategy used by an AssetSource. */
	enum class AssetStorage
	{
		FilePath,
		EmbeddedData,
		PackagedArchive
	};

	/** Describes where an asset's bytes come from before the runtime imports them. */
	struct AssetSource
	{
		AssetStorage storage = AssetStorage::FilePath;
		bool hot_reload = false;
		std::string location;
		std::string archive_path;
		std::vector<std::uint8_t> embedded_data;

		/** Creates a source that loads bytes from a filesystem path. */
		[[nodiscard]] static AssetSource File(std::string path);

		/** Creates a source that owns the asset bytes directly in memory. */
		[[nodiscard]] static AssetSource Embedded(std::vector<std::uint8_t> bytes);

		/** Creates a source that loads an entry from a packaged archive. */
		[[nodiscard]] static AssetSource Archive(std::string archive_path, std::string entry_path);

		/** Marks the source as eligible for reload when the backing data changes. */
		AssetSource& EnableHotReload(bool enabled = true) noexcept;
	};

	/** Named texture entry registered in an AssetCatalog. */
	struct TextureAsset
	{
		std::string name;
		std::string path;
		AssetSource source{};
	};

	/** Named model entry registered in an AssetCatalog. */
	struct ModelAsset
	{
		std::string name;
		std::string path;
		AssetSource source{};
	};

	/** Named sound entry registered in an AssetCatalog. */
	struct SoundAsset
	{
		std::string name;
		std::string path;
		AssetSource source{};
	};

	/** User-facing list of asset search roots and named assets used by EngineConfig. */
	struct AssetCatalog
	{
		std::vector<std::string> search_roots;
		std::vector<TextureAsset> textures;
		std::vector<ModelAsset> models;
		std::vector<SoundAsset> sounds;

		/** Adds a directory that relative asset paths may be resolved against. */
		AssetCatalog& SearchRoot(std::string path);

		/** Registers a texture asset by filesystem path. */
		AssetCatalog& Texture(std::string name, std::string path);

		/** Registers a texture asset by explicit source policy. */
		AssetCatalog& Texture(std::string name, AssetSource source);

		/** Registers a model asset by filesystem path. */
		AssetCatalog& Model(std::string name, std::string path);

		/** Registers a model asset by explicit source policy. */
		AssetCatalog& Model(std::string name, AssetSource source);

		/** Registers a sound asset by filesystem path. */
		AssetCatalog& Sound(std::string name, std::string path);

		/** Registers a sound asset by explicit source policy. */
		AssetCatalog& Sound(std::string name, AssetSource source);

		/** Returns validation messages; optional filesystem checks report missing paths and query failures. */
		[[nodiscard]] std::vector<std::string> Validate(bool require_existing_files = false) const;
	};
	/** @} */
}
