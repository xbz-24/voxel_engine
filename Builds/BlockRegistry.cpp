#include "BlockRegistry.h"

#include "BlockDefinitions.h"
#include "BlockTextureCache.h"

namespace ve::blocks
{
	namespace
	{
		/**
		 * Converts a block id into an array index.
		 */
		constexpr std::size_t IndexOf(BlockId id)
		{
			return static_cast<std::size_t>(id);
		}

		/**
		 * Converts a face id into an array index.
		 */
		constexpr std::size_t IndexOf(BlockFace face)
		{
			return static_cast<std::size_t>(face);
		}

		/**
		 * Loads all face textures for a block definition.
		 */
		std::array<GLuint, static_cast<std::size_t>(BlockFace::Count)> LoadFaces(BlockTextureCache& cache, const FaceTextureFiles& files)
		{
			return {
				cache.Load(files.top),
				cache.Load(files.bottom),
				cache.Load(files.front),
				cache.Load(files.back),
				cache.Load(files.left),
				cache.Load(files.right)
			};
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
				LoadFaces(cache, definition.textures)
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
}
