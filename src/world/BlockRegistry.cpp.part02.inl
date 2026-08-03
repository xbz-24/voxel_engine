
		BlockFaceTextureHandles LoadDataFaces(
			BlockTextureCache& cache,
			const DataBlockDefinition& definition,
			BlockRegistry::TextureLoading texture_loading)
		{
			return texture_loading == BlockRegistry::TextureLoading::LoadTextures
				? LoadFaces(cache, definition)
				: EmptyFaces();
		}

		BlockType BuildDataBlockType(
			BlockTextureCache& cache,
			const DataBlockDefinition& definition,
			BlockRegistry::TextureLoading texture_loading)
		{
			return BlockType{
				definition.id,
				definition.name,
				definition.is_solid,
				LoadDataFaces(cache, definition, texture_loading),
				definition.material,
				NormalizeDataGameplay(definition)
			};
		}
	}

	bool IsUsableBlockId(BlockId id) noexcept
	{
		return IsStoredBlockId(id);
	}

	BlockRegistry::BlockRegistry(const ve::assets::AssetPaths& paths, TextureLoading texture_loading)
	{
		BlockTextureCache cache(paths.blockTexturesDirectory);
		for (const BlockDefinition& definition : BuiltInBlockDefinitions)
		{
			(void)Register(BlockType{
				definition.id,
				std::string{ definition.name },
				definition.isSolid,
				texture_loading == TextureLoading::LoadTextures ? LoadFaces(cache, definition.textures) : EmptyFaces(),
				DefaultPbrMaterialForBlock(definition.id),
				DefaultGameplayFor(definition)
			});
		}
	}

	bool BlockRegistry::Register(BlockType block_type)
	{
		if (!IsUsableBlockId(block_type.id))
		{
			return false;
		}
		_blocks.insert_or_assign(block_type.id, std::move(block_type));
		return true;
	}

	bool BlockRegistry::RegisterDataDefinition(
		const DataBlockDefinition& definition,
		const ve::assets::AssetPaths& paths,
		TextureLoading texture_loading)
	{
		if (!CanRegisterDataDefinition(definition))
		{
			return false;
		}

		BlockTextureCache cache(paths.blockTexturesDirectory);
		return Register(BuildDataBlockType(cache, definition, texture_loading));
	}

	std::size_t BlockRegistry::RegisterDataDefinitions(
		std::span<const DataBlockDefinition> definitions,
		const ve::assets::AssetPaths& paths,
		TextureLoading texture_loading)
	{
		std::size_t accepted_definition_count = 0;
		BlockTextureCache cache(paths.blockTexturesDirectory);
		for (const DataBlockDefinition& definition : definitions)
		{
			if (!CanRegisterDataDefinition(definition))
			{
				continue;
			}
			if (Register(BuildDataBlockType(cache, definition, texture_loading)))
			{
				++accepted_definition_count;
