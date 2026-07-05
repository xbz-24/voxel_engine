#include "BlockRegistry.h"

#include "BlockDefinitions.h"
#include "BlockPbrMaterials.h"
#include "BlockTextureCache.h"
#include "CoreTypes.h"

#include <utility>

namespace ve::blocks
{
	namespace
	{
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
		std::array<ve::rendering::TextureHandle, static_cast<std::size_t>(BlockFace::Count)> LoadFaces(BlockTextureCache& cache, const FaceTextureFiles& files)
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

		std::array<ve::rendering::TextureHandle, static_cast<std::size_t>(BlockFace::Count)> EmptyFaces() noexcept
		{
			std::array<ve::rendering::TextureHandle, static_cast<std::size_t>(BlockFace::Count)> faces{};
			faces.fill(ve::rendering::kInvalidTextureHandle);
			return faces;
		}

		bool IsUsableBlockId(BlockId id) noexcept
		{
			return id != BlockId::Count;
		}
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
				DefaultPbrMaterialForBlock(definition.id)
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

	bool BlockRegistry::Contains(BlockId id) const
	{
		return _blocks.find(id) != _blocks.end();
	}

	std::size_t BlockRegistry::RegisteredBlockCount() const noexcept
	{
		return _blocks.size();
	}

	const BlockType& BlockRegistry::Get(BlockId id) const
	{
		const auto block_iterator = _blocks.find(id);
		return block_iterator != _blocks.end() ? block_iterator->second : FallbackBlock();
	}

	bool BlockRegistry::IsAir(BlockId id) const
	{
		return id == BlockId::Air;
	}

	bool BlockRegistry::IsSolid(BlockId id) const
	{
		return Get(id).isSolid;
	}

	ve::rendering::TextureHandle BlockRegistry::TextureFor(BlockId id, BlockFace face) const
	{
		return Get(id).faceTextures[IndexOf(face)];
	}

	const ve::rendering::PbrMaterial& BlockRegistry::MaterialFor(BlockId id) const
	{
		return Get(id).material;
	}

	const BlockType& BlockRegistry::FallbackBlock() const noexcept
	{
		const auto air_iterator = _blocks.find(BlockId::Air);
		if (air_iterator != _blocks.end())
		{
			return air_iterator->second;
		}

		static const BlockType fallback_air_block{
			BlockId::Air,
			"Air",
			false,
			EmptyFaces(),
			ve::rendering::PbrMaterial{}
		};
		return fallback_air_block;
	}
}
