#include "BlockTextureCache.h"

#include "TextureLoader.h"

namespace ve::blocks
{
	BlockTextureCache::BlockTextureCache(const std::filesystem::path& blockTextureDirectory)
		: _blockTextureDirectory(blockTextureDirectory)
	{
	}

	ve::rendering::TextureHandle BlockTextureCache::Load(const char* fileName)
	{
		if (fileName == nullptr)
		{
			return ve::rendering::kInvalidTextureHandle;
		}

		auto [entry, inserted] = _textures.try_emplace(fileName, ve::rendering::kInvalidTextureHandle);
		if (inserted)
		{
			entry->second = ve::rendering::LoadTexture((_blockTextureDirectory / fileName).string().c_str());
		}
		return entry->second;
	}
}
