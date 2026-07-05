namespace voxel
{
	namespace
	{
		[[nodiscard]] std::string AssetPathFromSource(const AssetSource& source)
		{
			return source.location;
		}

		[[nodiscard]] float Clamp01(float value) noexcept
		{
			return std::clamp(value, 0.0f, 1.0f);
		}

		[[nodiscard]] Color ClampColor(Color value) noexcept
		{
			return {
				Clamp01(value.r),
				Clamp01(value.g),
				Clamp01(value.b),
				Clamp01(value.a)
			};
		}
	}

	AssetSource AssetSource::File(std::string path)
	{
		AssetSource source{};
		source.storage = AssetStorage::FilePath;
		source.location = std::move(path);
		return source;
	}

	AssetSource AssetSource::Embedded(std::vector<std::uint8_t> bytes)
	{
		AssetSource source{};
		source.storage = AssetStorage::EmbeddedData;
		source.embedded_data = std::move(bytes);
		return source;
	}

	AssetSource AssetSource::Archive(std::string archive_path, std::string entry_path)
	{
		AssetSource source{};
		source.storage = AssetStorage::PackagedArchive;
		source.archive_path = std::move(archive_path);
		source.location = std::move(entry_path);
		return source;
	}

	AssetSource& AssetSource::EnableHotReload(bool enabled) noexcept
	{
		hot_reload = enabled;
		return *this;
	}

	AssetCatalog& AssetCatalog::SearchRoot(std::string path)
	{
		search_roots.push_back(std::move(path));
		return *this;
	}

	AssetCatalog& AssetCatalog::Texture(std::string name, std::string path)
	{
		return Texture(std::move(name), AssetSource::File(std::move(path)));
	}

	AssetCatalog& AssetCatalog::Texture(std::string name, AssetSource source)
	{
		textures.push_back(TextureAsset{ std::move(name), AssetPathFromSource(source), std::move(source) });
		return *this;
	}

	AssetCatalog& AssetCatalog::Model(std::string name, std::string path)
	{
		return Model(std::move(name), AssetSource::File(std::move(path)));
	}

	AssetCatalog& AssetCatalog::Model(std::string name, AssetSource source)
	{
		models.push_back(ModelAsset{ std::move(name), AssetPathFromSource(source), std::move(source) });
		return *this;
	}

	AssetCatalog& AssetCatalog::Sound(std::string name, std::string path)
	{
		return Sound(std::move(name), AssetSource::File(std::move(path)));
	}

	AssetCatalog& AssetCatalog::Sound(std::string name, AssetSource source)
	{
		sounds.push_back(SoundAsset{ std::move(name), AssetPathFromSource(source), std::move(source) });
		return *this;
	}
