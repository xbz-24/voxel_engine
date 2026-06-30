namespace voxel
{
	namespace
	{
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

	AssetCatalog& AssetCatalog::SearchRoot(std::string path)
	{
		search_roots.push_back(std::move(path));
		return *this;
	}

	AssetCatalog& AssetCatalog::Texture(std::string name, std::string path)
	{
		textures.push_back(TextureAsset{ std::move(name), std::move(path) });
		return *this;
	}

	AssetCatalog& AssetCatalog::Model(std::string name, std::string path)
	{
		models.push_back(ModelAsset{ std::move(name), std::move(path) });
		return *this;
	}

	AssetCatalog& AssetCatalog::Sound(std::string name, std::string path)
	{
		sounds.push_back(SoundAsset{ std::move(name), std::move(path) });
		return *this;
	}
