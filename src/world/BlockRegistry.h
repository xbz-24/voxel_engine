#pragma once

#include "AssetPaths.h"
#include "Block.h"
#include "GraphicsTypes.h"
#include "PbrMaterial.h"

#include <array>
#include <string_view>

namespace ve::blocks
{
	/**
	 * Rendering/gameplay metadata for one block type.
	 */
	struct BlockType
	{
		BlockId id;
		std::string_view name;
		bool isSolid;
		std::array<ve::rendering::TextureHandle, static_cast<std::size_t>(BlockFace::Count)> faceTextures;
		ve::rendering::PbrMaterial material;
	};

	/**
	 * Owns block definitions and their loaded texture ids.
	 */
	class BlockRegistry
	{
	public:
		/**
		 * Loads built-in block definitions from the asset tree.
		 *
		 * @param paths Resolved engine asset paths.
		 */
		explicit BlockRegistry(const ve::assets::AssetPaths& paths);

		/**
		 * Returns metadata for a block id.
		 *
		 * @param id Block id to look up.
		 * @return Stable block type metadata.
		 */
		const BlockType& Get(BlockId id) const;

		/**
		 * Returns whether a block id is air.
		 *
		 * @param id Block id to inspect.
		 * @return True when the id represents air.
		 */
		bool IsAir(BlockId id) const;

		/**
		 * Returns whether a block id should collide and occlude faces.
		 *
		 * @param id Block id to inspect.
		 * @return True when the block is solid.
		 */
		bool IsSolid(BlockId id) const;

		/**
		 * Returns the texture used by a block face.
		 *
		 * @param id Block id to inspect.
		 * @param face Face whose texture is requested.
		 * @return Backend-neutral texture handle, or zero for air/invalid data.
		 */
		ve::rendering::TextureHandle TextureFor(BlockId id, BlockFace face) const;

		/**
		 * Returns the physically based material used by a block.
		 *
		 * @param id Block id to inspect.
		 * @return PBR material values for future shader-based rendering.
		 */
		const ve::rendering::PbrMaterial& MaterialFor(BlockId id) const;

	private:
		std::array<BlockType, static_cast<std::size_t>(BlockId::Count)> _blocks;
	};
}
