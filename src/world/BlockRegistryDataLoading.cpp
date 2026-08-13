#include "BlockRegistryLoading.h"

#include "BlockTextureCache.h"

namespace ve::blocks::detail
{
	namespace
	{
		BlockFaceTextureHandles LoadDataFaces(
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

		BlockGameplayProperties NormalizeGameplay(const DataBlockDefinition& definition)
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
	}

	bool CanRegisterDataDefinition(const DataBlockDefinition& definition) noexcept
	{
		return IsUsableBlockId(definition.id) && !definition.name.empty();
	}

	BlockType BuildDataBlockType(
		BlockTextureCache& cache,
		const DataBlockDefinition& definition,
		BlockRegistry::TextureLoading texture_loading)
	{
		const BlockFaceTextureHandles faces = texture_loading == BlockRegistry::TextureLoading::LoadTextures
			? LoadDataFaces(cache, definition)
			: EmptyFaces();
		return BlockType{
			definition.id,
			definition.name,
			definition.is_solid,
			faces,
			definition.material,
			NormalizeGameplay(definition)
		};
	}
}
