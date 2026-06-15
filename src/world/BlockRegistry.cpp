#include "BlockRegistry.h"

#include "BlockDefinitions.h"
#include "BlockPbrMaterials.h"
#include "BlockTextureCache.h"
#include "CoreTypes.h"

namespace ve::blocks
{
	namespace
	{
		/**
		 * Converts a block id into an array index.
		 */
		constexpr std::size_t IndexOf(BlockId id)
		{
			return ve::core::ToIndex(id);
		}

		/**
		 * Converts a face id into an array index.
		 */
		constexpr std::size_t IndexOf(BlockFace face)
		{
			return ve::core::ToIndex(face);
		}

		/**
		 * Loads all face textures for a block definition.
		 */
		std::array<GLuint, static_cast<std::size_t>(BlockFace::Count)> LoadFaces(BlockTextureCache& cache, const FaceTextureFiles& files)
		{
			return { {
				cache.Load(files.top),
				cache.Load(files.bottom),
				cache.Load(files.front),
				cache.Load(files.back),
				cache.Load(files.left),
				cache.Load(files.right)
			} };
		}
	}

	BlockRegistry::BlockRegistry(const ve::assets::AssetPaths& paths)
	{
		BlockTextureCache cache(paths.blockTexturesDirectory);
		for (const BlockDefinition& definition : BuiltInBlockDefinitions)
		{
			_blocks[IndexOf(definition.id)] = BlockType{
				definition.id,
				definition.name,
				definition.isSolid,
				LoadFaces(cache, definition.textures),
				DefaultPbrMaterialForBlock(definition.id)
			};
		}
	}

	const BlockType& BlockRegistry::Get(BlockId id) const
	{
		return _blocks[IndexOf(id)];
	}

	bool BlockRegistry::IsAir(BlockId id) const
	{
		return id == BlockId::Air;
	}

	bool BlockRegistry::IsSolid(BlockId id) const
	{
		return Get(id).isSolid;
	}

	GLuint BlockRegistry::TextureFor(BlockId id, BlockFace face) const
	{
		return Get(id).faceTextures[IndexOf(face)];
	}

	const ve::rendering::PbrMaterial& BlockRegistry::MaterialFor(BlockId id) const
	{
		return Get(id).material;
	}
}
