#pragma once

#include "AssetPaths.h"
#include "BlockRegistryTypes.h"

#include <cstddef>
#include <span>
#include <unordered_map>

namespace ve::blocks
{
	class BlockRegistry
	{
	public:
		enum class TextureLoading
		{
			LoadTextures,
			MetadataOnly
		};

		explicit BlockRegistry(
			const ve::assets::AssetPaths& paths,
			TextureLoading texture_loading = TextureLoading::LoadTextures);

		bool Register(BlockType block_type);
		bool RegisterDataDefinition(
			const DataBlockDefinition& definition,
			const ve::assets::AssetPaths& paths,
			TextureLoading texture_loading = TextureLoading::LoadTextures);
		std::size_t RegisterDataDefinitions(
			std::span<const DataBlockDefinition> definitions,
			const ve::assets::AssetPaths& paths,
			TextureLoading texture_loading = TextureLoading::LoadTextures);

		bool Contains(BlockId id) const;
		std::size_t RegisteredBlockCount() const noexcept;
		const BlockType& Get(BlockId id) const;
		bool IsAir(BlockId id) const;
		bool IsSolid(BlockId id) const;
		ve::rendering::TextureHandle TextureFor(BlockId id, BlockFace face) const;
		const ve::rendering::PbrMaterial& MaterialFor(BlockId id) const;
		const BlockGameplayProperties& GameplayFor(BlockId id) const;
		bool BlocksMovement(BlockId id) const;
		bool IsTransparent(BlockId id) const;
		bool IsRenderable(BlockId id) const;
		bool OccludesNeighborFaces(BlockId id) const;

	private:
		const BlockType& FallbackBlock() const noexcept;
		std::unordered_map<BlockId, BlockType> _blocks;
	};

	[[nodiscard]] bool IsUsableBlockId(BlockId id) noexcept;
}
