#include "BlockRegistry.h"

namespace ve::blocks
{
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
}
