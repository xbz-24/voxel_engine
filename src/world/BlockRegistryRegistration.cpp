#include "BlockRegistry.h"

#include "BlockDefinitions.h"
#include "BlockPbrMaterials.h"
#include "BlockRegistryLoading.h"
#include "BlockTextureCache.h"

namespace ve::blocks
{
	BlockRegistry::BlockRegistry(const ve::assets::AssetPaths& paths, TextureLoading texture_loading)
	{
		BlockTextureCache cache(paths.blockTexturesDirectory);
		for (const BlockDefinition& definition : BuiltInBlockDefinitions)
		{
			const BlockFaceTextureHandles faces = texture_loading == TextureLoading::LoadTextures
				? detail::LoadBuiltInFaces(cache, definition.textures)
				: detail::EmptyFaces();
			(void)Register(BlockType{
				definition.id,
				std::string{ definition.name },
				definition.isSolid,
				faces,
				DefaultPbrMaterialForBlock(definition.id),
				detail::DefaultGameplayFor(definition)
			});
		}
	}

	bool BlockRegistry::RegisterDataDefinition(
		const DataBlockDefinition& definition,
		const ve::assets::AssetPaths& paths,
		TextureLoading texture_loading)
	{
		if (!detail::CanRegisterDataDefinition(definition)) return false;
		BlockTextureCache cache(paths.blockTexturesDirectory);
		return Register(detail::BuildDataBlockType(cache, definition, texture_loading));
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
			if (detail::CanRegisterDataDefinition(definition) &&
				Register(detail::BuildDataBlockType(cache, definition, texture_loading)))
			{
				++accepted_definition_count;
			}
		}
		return accepted_definition_count;
	}
}
