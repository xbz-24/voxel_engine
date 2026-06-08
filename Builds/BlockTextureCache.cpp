#include "BlockTextureCache.h"

#include "Utilities.h"

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
			entry->second = Utils::load_texture((_blockTextureDirectory / fileName).string().c_str());
		}
		return entry->second;
	}
}
