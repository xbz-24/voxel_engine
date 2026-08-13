#pragma once

#include "Block.h"
#include "GraphicsTypes.h"
#include "PbrMaterial.h"

#include <cstdint>
#include <string>
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

	struct BlockType
	{
		BlockId id;
		std::string name;
		bool isSolid;
		BlockFaceTextureHandles faceTextures;
		ve::rendering::PbrMaterial material;
		BlockGameplayProperties gameplay{};
	};
}
