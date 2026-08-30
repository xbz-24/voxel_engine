#include "BlockRegistryLoading.h"

#include "BlockTextureCache.h"

namespace ve::blocks::detail
{
	BlockFaceTextureHandles EmptyFaces() noexcept
	{
		BlockFaceTextureHandles faces{};
		faces.fill(ve::rendering::kInvalidTextureHandle);
		return faces;
	}

	BlockFaceTextureHandles LoadBuiltInFaces(
		BlockTextureCache& cache,
		const FaceTextureFiles& files)
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

	BlockGameplayProperties DefaultGameplayFor(const BlockDefinition& definition)
	{
		BlockGameplayProperties properties{};
		properties.collision = definition.isSolid
			? BlockCollisionMode::Solid
			: BlockCollisionMode::None;
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
}
