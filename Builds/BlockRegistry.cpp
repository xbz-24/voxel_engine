#include "BlockRegistry.h"

#include "Utilities.h"

#include <filesystem>

namespace ve::blocks
{
	namespace
	{
		/**
		 * Converts a block id into its array index.
		 *
		 * @param id Block id to convert.
		 * @return Index into the registry array.
		 */
		std::size_t IndexOf(BlockId id)
		{
			return static_cast<std::size_t>(id);
		}

		/**
		 * Converts a face id into its array index.
		 *
		 * @param face Block face to convert.
		 * @return Index into a face texture array.
		 */
		std::size_t IndexOf(BlockFace face)
		{
			return static_cast<std::size_t>(face);
		}

		/**
		 * Builds a texture array where all faces use the same texture.
		 *
		 * @param texture OpenGL texture id used by every face.
		 * @return Face texture array.
		 */
		std::array<GLuint, static_cast<std::size_t>(BlockFace::Count)> SameTexture(GLuint texture)
		{
			return { texture, texture, texture, texture, texture, texture };
		}

		/**
		 * Loads a texture from the block texture directory.
		 *
		 * @param blockTextureDirectory Directory containing block textures.
		 * @param fileName Texture file name.
		 * @return OpenGL texture id, or 0 when loading fails.
		 */
		GLuint LoadBlockTexture(const std::filesystem::path& blockTextureDirectory, const char* fileName)
		{
			return Utils::load_texture((blockTextureDirectory / fileName).string().c_str());
		}
	}

	BlockRegistry::BlockRegistry(const ve::assets::AssetPaths& paths)
	{
		const GLuint grassTop = LoadBlockTexture(paths.blockTexturesDirectory, "grass_block_top.png");
		const GLuint grassSide = LoadBlockTexture(paths.blockTexturesDirectory, "grass_block_side.png");
		const GLuint dirt = LoadBlockTexture(paths.blockTexturesDirectory, "dirt.png");
		const GLuint stone = LoadBlockTexture(paths.blockTexturesDirectory, "stone.png");
		const GLuint cobblestone = LoadBlockTexture(paths.blockTexturesDirectory, "cobblestone.png");
		const GLuint oakLog = LoadBlockTexture(paths.blockTexturesDirectory, "oak_log.png");
		const GLuint oakLogTop = LoadBlockTexture(paths.blockTexturesDirectory, "oak_log_top.png");
		const GLuint oakPlanks = LoadBlockTexture(paths.blockTexturesDirectory, "oak_planks.png");
		const GLuint sand = LoadBlockTexture(paths.blockTexturesDirectory, "sand.png");
		const GLuint bricks = LoadBlockTexture(paths.blockTexturesDirectory, "bricks.png");
		const GLuint diamondOre = LoadBlockTexture(paths.blockTexturesDirectory, "diamond_ore.png");
		const GLuint coalOre = LoadBlockTexture(paths.blockTexturesDirectory, "coal_ore.png");
		const GLuint ironOre = LoadBlockTexture(paths.blockTexturesDirectory, "iron_ore.png");
		const GLuint obsidian = LoadBlockTexture(paths.blockTexturesDirectory, "obsidian.png");
		const GLuint oakLeaves = LoadBlockTexture(paths.blockTexturesDirectory, "oak_leaves.png");
		const GLuint deepslate = LoadBlockTexture(paths.blockTexturesDirectory, "deepslate.png");
		const GLuint copperOre = LoadBlockTexture(paths.blockTexturesDirectory, "copper_ore.png");
		const GLuint goldOre = LoadBlockTexture(paths.blockTexturesDirectory, "gold_ore.png");
		const GLuint emeraldOre = LoadBlockTexture(paths.blockTexturesDirectory, "emerald_ore.png");
		const GLuint lapisOre = LoadBlockTexture(paths.blockTexturesDirectory, "lapis_ore.png");
		const GLuint gravel = LoadBlockTexture(paths.blockTexturesDirectory, "gravel.png");
		const GLuint mossBlock = LoadBlockTexture(paths.blockTexturesDirectory, "moss_block.png");
		const GLuint snow = LoadBlockTexture(paths.blockTexturesDirectory, "snow.png");
		const GLuint mossyCobblestone = LoadBlockTexture(paths.blockTexturesDirectory, "mossy_cobblestone.png");

		_blocks[IndexOf(BlockId::Air)] = BlockType{
			BlockId::Air,
			"Air",
			false,
			SameTexture(0)
		};

		_blocks[IndexOf(BlockId::Grass)] = BlockType{
			BlockId::Grass,
			"Grass",
			true,
			{
				grassTop,
				dirt,
				grassSide,
				grassSide,
				grassSide,
				grassSide
			}
		};

		_blocks[IndexOf(BlockId::Dirt)] = BlockType{
			BlockId::Dirt,
			"Dirt",
			true,
			SameTexture(dirt)
		};

		_blocks[IndexOf(BlockId::Stone)] = BlockType{
			BlockId::Stone,
			"Stone",
			true,
			SameTexture(stone)
		};

		_blocks[IndexOf(BlockId::Cobblestone)] = BlockType{
			BlockId::Cobblestone,
			"Cobblestone",
			true,
			SameTexture(cobblestone)
		};

		_blocks[IndexOf(BlockId::OakLog)] = BlockType{
			BlockId::OakLog,
			"Oak Log",
			true,
			{
				oakLogTop,
				oakLogTop,
				oakLog,
				oakLog,
				oakLog,
				oakLog
			}
		};

		_blocks[IndexOf(BlockId::OakPlanks)] = BlockType{
			BlockId::OakPlanks,
			"Oak Planks",
			true,
			SameTexture(oakPlanks)
		};

		_blocks[IndexOf(BlockId::Sand)] = BlockType{
			BlockId::Sand,
			"Sand",
			true,
			SameTexture(sand)
		};

		_blocks[IndexOf(BlockId::Bricks)] = BlockType{
			BlockId::Bricks,
			"Bricks",
			true,
			SameTexture(bricks)
		};

		_blocks[IndexOf(BlockId::DiamondOre)] = BlockType{
			BlockId::DiamondOre,
			"Diamond Ore",
			true,
			SameTexture(diamondOre)
		};

		_blocks[IndexOf(BlockId::CoalOre)] = BlockType{
			BlockId::CoalOre,
			"Coal Ore",
			true,
			SameTexture(coalOre)
		};

		_blocks[IndexOf(BlockId::IronOre)] = BlockType{
			BlockId::IronOre,
			"Iron Ore",
			true,
			SameTexture(ironOre)
		};

		_blocks[IndexOf(BlockId::Obsidian)] = BlockType{
			BlockId::Obsidian,
			"Obsidian",
			true,
			SameTexture(obsidian)
		};

		_blocks[IndexOf(BlockId::OakLeaves)] = BlockType{
			BlockId::OakLeaves,
			"Oak Leaves",
			true,
			SameTexture(oakLeaves)
		};

		_blocks[IndexOf(BlockId::Deepslate)] = BlockType{
			BlockId::Deepslate,
			"Deepslate",
			true,
			SameTexture(deepslate)
		};

		_blocks[IndexOf(BlockId::CopperOre)] = BlockType{
			BlockId::CopperOre,
			"Copper Ore",
			true,
			SameTexture(copperOre)
		};

		_blocks[IndexOf(BlockId::GoldOre)] = BlockType{
			BlockId::GoldOre,
			"Gold Ore",
			true,
			SameTexture(goldOre)
		};

		_blocks[IndexOf(BlockId::EmeraldOre)] = BlockType{
			BlockId::EmeraldOre,
			"Emerald Ore",
			true,
			SameTexture(emeraldOre)
		};

		_blocks[IndexOf(BlockId::LapisOre)] = BlockType{
			BlockId::LapisOre,
			"Lapis Ore",
			true,
			SameTexture(lapisOre)
		};

		_blocks[IndexOf(BlockId::Gravel)] = BlockType{
			BlockId::Gravel,
			"Gravel",
			true,
			SameTexture(gravel)
		};

		_blocks[IndexOf(BlockId::MossBlock)] = BlockType{
			BlockId::MossBlock,
			"Moss",
			true,
			SameTexture(mossBlock)
		};

		_blocks[IndexOf(BlockId::Snow)] = BlockType{
			BlockId::Snow,
			"Snow",
			true,
			SameTexture(snow)
		};

		_blocks[IndexOf(BlockId::MossyCobblestone)] = BlockType{
			BlockId::MossyCobblestone,
			"Mossy Cobble",
			true,
			SameTexture(mossyCobblestone)
		};
	}

	const BlockType& BlockRegistry::Get(BlockId id) const
	{
		return _blocks[IndexOf(id)];
	}

	bool BlockRegistry::IsAir(BlockId id) const
	{
		return id == BlockId::Air;
	}

	bool BlockRegistry::IsSolid(BlockId id) const
	{
		return Get(id).isSolid;
	}

	GLuint BlockRegistry::TextureFor(BlockId id, BlockFace face) const
	{
		return Get(id).faceTextures[IndexOf(face)];
	}
}
