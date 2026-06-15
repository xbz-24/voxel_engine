#pragma once

#include <GL/glew.h>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace ve::blocks
{
	class BlockTextureCache
	{
	public:
		/**
		 * Creates a cache rooted in the block texture directory.
		 *
		 * @param blockTextureDirectory Directory containing block PNG files.
		 */
		explicit BlockTextureCache(const std::filesystem::path& blockTextureDirectory);

		/**
		 * Loads or returns an existing OpenGL texture.
		 *
		 * @param fileName Texture file name, or nullptr for no texture.
		 * @return OpenGL texture id.
		 */
		GLuint Load(const char* fileName);

	private:
		std::filesystem::path _blockTextureDirectory;
		std::unordered_map<std::string, GLuint> _textures;
	};
}
