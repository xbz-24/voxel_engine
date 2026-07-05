#pragma once

#include "AssetPaths.h"
#include "Block.h"
#include "GraphicsTypes.h"
#include "PbrMaterial.h"

#include <array>
#include <cstddef>
#include <string>
#include <unordered_map>

namespace ve::blocks
{
	/**
	 * Rendering/gameplay metadata for one block type.
	 */
	struct BlockType
	{
		// TODO: Load block definitions from data so API users can register custom blocks without recompiling.
		BlockId id;
		std::string name;
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
		enum class TextureLoading
		{
			LoadTextures,
			MetadataOnly
		};

		// TODO: Add public extension points for custom block ids, collision flags, transparency, sounds, and drops.
		/**
		 * Loads built-in block definitions from the asset tree.
		 *
		 * @param paths Resolved engine asset paths.
		 * @param texture_loading Whether legacy face textures should be uploaded.
		 */
		explicit BlockRegistry(const ve::assets::AssetPaths& paths, TextureLoading texture_loading = TextureLoading::LoadTextures);

		/**
		 * Registers or replaces metadata for a stable block id.
		 *
		 * @param block_type Metadata to store.
		 * @return True when the block id is usable by world storage.
		 */
		bool Register(BlockType block_type);

		/**
		 * Returns whether metadata exists for a block id.
		 *
		 * @param id Block id to look up.
		 * @return True when the registry has metadata for the id.
		 */
		bool Contains(BlockId id) const;

		/**
		 * Returns how many block definitions are registered.
		 *
		 * @return Registered block definition count.
		 */
		std::size_t RegisteredBlockCount() const noexcept;

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
		const BlockType& FallbackBlock() const noexcept;

		std::unordered_map<BlockId, BlockType> _blocks;
	};
}
