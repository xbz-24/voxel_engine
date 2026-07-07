#pragma once

#include "AssetPaths.h"
#include "Block.h"
#include "GraphicsTypes.h"
#include "PbrMaterial.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace ve::blocks
{
	enum class BlockCollisionMode
	{
		None,
		Solid
	};

	struct BlockDrop
	{
		BlockId id = BlockId::Air;
		std::uint8_t minimum_count = 1;
		std::uint8_t maximum_count = 1;
	};

	struct BlockGameplayProperties
	{
		BlockCollisionMode collision = BlockCollisionMode::None;
		bool transparent = true;
		std::string footstep_sound;
		std::string break_sound;
		std::vector<BlockDrop> drops;
	};

	struct DataBlockDefinition
	{
		BlockId id = BlockId::Air;
		std::string name;
		bool is_solid = false;
		std::array<std::string, static_cast<std::size_t>(BlockFace::Count)> face_texture_files{};
		ve::rendering::PbrMaterial material;
		BlockGameplayProperties gameplay{};
	};

	/**
	 * Rendering/gameplay metadata for one block type.
	 */
	struct BlockType
	{
		BlockId id;
		std::string name;
		bool isSolid;
		std::array<ve::rendering::TextureHandle, static_cast<std::size_t>(BlockFace::Count)> faceTextures;
		ve::rendering::PbrMaterial material;
		BlockGameplayProperties gameplay{};
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
		 * Registers or replaces metadata described by data files or editor tools.
		 *
		 * @param definition Data-authored block definition.
		 * @param paths Resolved engine asset paths used to load face textures.
		 * @param texture_loading Whether face textures should be uploaded.
		 * @return True when the definition is usable by world storage.
		 */
		bool RegisterDataDefinition(
			const DataBlockDefinition& definition,
			const ve::assets::AssetPaths& paths,
			TextureLoading texture_loading = TextureLoading::LoadTextures);

		/**
		 * Registers a batch of data-authored block definitions.
		 *
		 * @param definitions Definitions to register.
		 * @param paths Resolved engine asset paths used to load face textures.
		 * @param texture_loading Whether face textures should be uploaded.
		 * @return Count of definitions accepted by the registry.
		 */
		std::size_t RegisterDataDefinitions(
			std::span<const DataBlockDefinition> definitions,
			const ve::assets::AssetPaths& paths,
			TextureLoading texture_loading = TextureLoading::LoadTextures);

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

		/** @param id Block id to inspect. @return Gameplay/collision metadata for the block. */
		const BlockGameplayProperties& GameplayFor(BlockId id) const;

		/** @param id Block id to inspect. @return True when the block stops player/world movement. */
		bool BlocksMovement(BlockId id) const;

		/** @param id Block id to inspect. @return True when the block should not fully occlude rendering/light. */
		bool IsTransparent(BlockId id) const;

		/** @param id Block id to inspect. @return True when the block should emit visible geometry. */
		bool IsRenderable(BlockId id) const;

		/** @param id Block id to inspect. @return True when the block hides a neighboring face. */
		bool OccludesNeighborFaces(BlockId id) const;

	private:
		const BlockType& FallbackBlock() const noexcept;

		std::unordered_map<BlockId, BlockType> _blocks;
	};

	/** @param id Block id to inspect. @return True when the built-in id can be stored in chunks. */
	[[nodiscard]] bool IsUsableBlockId(BlockId id) noexcept;
}
