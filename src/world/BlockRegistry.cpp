#include "BlockRegistry.h"

#include "BlockDefinitions.h"
#include "BlockPbrMaterials.h"
#include "BlockTextureCache.h"

#include <utility>

namespace ve::blocks
{
	namespace
	{
		/**
		 * Loads all face textures for a block definition.
		 */
		BlockFaceTextureHandles LoadFaces(BlockTextureCache& cache, const FaceTextureFiles& files)
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

		BlockFaceTextureHandles EmptyFaces() noexcept
		{
			BlockFaceTextureHandles faces{};
			faces.fill(ve::rendering::kInvalidTextureHandle);
			return faces;
		}

		BlockGameplayProperties DefaultGameplayFor(const BlockDefinition& definition)
		{
			BlockGameplayProperties properties{};
			properties.collision = definition.isSolid ? BlockCollisionMode::Solid : BlockCollisionMode::None;
			properties.transparent = !definition.isSolid ||
				definition.id == BlockId::Glass ||
				definition.id == BlockId::Water ||
				definition.id == BlockId::OakLeaves ||
				definition.id == BlockId::BirchLeaves ||
				definition.id == BlockId::CherryLeaves;
			if (definition.id != BlockId::Air)
			{
				properties.drops.push_back(BlockDrop{ definition.id, 1, 1 });
			}
			return properties;
		}

		BlockFaceTextureHandles LoadFaces(
			BlockTextureCache& cache,
			const DataBlockDefinition& definition)
		{
			BlockFaceTextureHandles faces{};
			for (BlockFace face : AllBlockFaces)
			{
				const BlockFaceOrdinal face_index = ToBlockFaceOrdinal(face);
				const std::string& texture_file = definition.face_texture_files[face_index];
				faces[face_index] = cache.Load(texture_file.empty() ? nullptr : texture_file.c_str());
			}
			return faces;
		}

		BlockGameplayProperties NormalizeDataGameplay(const DataBlockDefinition& definition)
		{
			BlockGameplayProperties gameplay = definition.gameplay;
			if (definition.is_solid && gameplay.collision == BlockCollisionMode::None)
			{
				gameplay.collision = BlockCollisionMode::Solid;
			}
			if (definition.is_solid && gameplay.transparent && gameplay.drops.empty() &&
				gameplay.footstep_sound.empty() && gameplay.break_sound.empty())
			{
				gameplay.transparent = false;
			}
			if (definition.id != BlockId::Air && gameplay.drops.empty())
			{
				gameplay.drops.push_back(BlockDrop{ definition.id, 1, 1 });
			}
			return gameplay;
		}

		bool CanRegisterDataDefinition(const DataBlockDefinition& definition) noexcept
		{
			return IsUsableBlockId(definition.id) && !definition.name.empty();
		}

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
			}
		}
		return accepted_definition_count;
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
		return Get(id).faceTextures[ToBlockFaceOrdinal(face)];
	}

	const ve::rendering::PbrMaterial& BlockRegistry::MaterialFor(BlockId id) const
	{
		return Get(id).material;
	}

	const BlockGameplayProperties& BlockRegistry::GameplayFor(BlockId id) const
	{
		return Get(id).gameplay;
	}

	bool BlockRegistry::BlocksMovement(BlockId id) const
	{
		return GameplayFor(id).collision == BlockCollisionMode::Solid;
	}

	bool BlockRegistry::IsTransparent(BlockId id) const
	{
		return GameplayFor(id).transparent;
	}

	bool BlockRegistry::IsRenderable(BlockId id) const
	{
		return id != BlockId::Air && Contains(id);
	}

	bool BlockRegistry::OccludesNeighborFaces(BlockId id) const
	{
		return IsRenderable(id) && !IsTransparent(id);
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
			ve::rendering::PbrMaterial{},
			BlockGameplayProperties{}
		};
		return fallback_air_block;
	}
}
