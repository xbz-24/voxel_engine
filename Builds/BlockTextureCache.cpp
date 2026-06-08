#include "BlockTextureCache.h"

#include "TextureLoader.h"

namespace ve::blocks
{
	BlockTextureCache::BlockTextureCache(const std::filesystem::path& blockTextureDirectory)
		: _blockTextureDirectory(blockTextureDirectory)
	{
	}

	GLuint BlockTextureCache::Load(const char* fileName)
	{
		if (fileName == nullptr)
		{
			return 0;
		}

		auto [entry, inserted] = _textures.try_emplace(fileName, 0);
		if (inserted)
		{
			entry->second = ve::rendering::LoadTexture((_blockTextureDirectory / fileName).string().c_str());
		}
		return entry->second;
	}
}
