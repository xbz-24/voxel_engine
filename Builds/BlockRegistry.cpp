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

