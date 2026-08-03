
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
		PerBlockFaceArray<std::string> face_texture_files{};
		ve::rendering::PbrMaterial material;
		BlockGameplayProperties gameplay{};
	};

	using BlockFaceTextureHandles = PerBlockFaceArray<ve::rendering::TextureHandle>;

	/**
	 * Rendering/gameplay metadata for one block type.
	 */
	struct BlockType
	{
		BlockId id;
		std::string name;
		bool isSolid;
		BlockFaceTextureHandles faceTextures;
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

