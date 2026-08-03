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
