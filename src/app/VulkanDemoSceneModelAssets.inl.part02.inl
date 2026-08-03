			if (profile.model_asset_keyword.empty())
			{
				return ModelAssetPreflight{
					std::nullopt,
					"Model demo profile has no asset keyword"
				};
			}
			const std::filesystem::path root = AssetRoot();
			if (!std::filesystem::exists(root))
			{
				return ModelAssetPreflight{
					std::nullopt,
					"Model asset root not found: " + root.string()
				};
			}
			std::optional<std::filesystem::path> path = FindModelAsset(profile.model_asset_keyword);
			if (!path)
			{
				return ModelAssetPreflight{
					std::nullopt,
					"Model asset '" + std::string{ profile.model_asset_keyword } + "' not found under " + root.string()
				};
			}
			return ModelAssetPreflight{ std::move(path), {} };
		}

		[[nodiscard]] std::uint64_t VoxelKey(int x, int y, int z) noexcept
		{
			return PackedSceneVoxelKey(x, y, z);
		}

		[[nodiscard]] std::uint32_t StableStringHash(std::string_view value) noexcept
		{
			std::uint32_t hash = 2166136261u;
			for (char ch : value)
			{
				hash ^= SceneByte(ch);
				hash *= 16777619u;
			}
			return hash;
		}

		template <std::size_t Count>
		[[nodiscard]] BlockId PickBlock(const std::array<BlockId, Count>& palette, std::uint32_t hash) noexcept
		{
			return palette[hash % Count];
		}
