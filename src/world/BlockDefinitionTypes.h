#pragma once

#include "Block.h"

#include <string_view>

namespace ve::blocks
{
	struct FaceTextureFiles
	{
		const char* top;
		const char* bottom;
		const char* front;
		const char* back;
		const char* left;
		const char* right;
	};

	struct BlockDefinition
	{
		BlockId id;
		std::string_view name;
		bool isSolid;
		FaceTextureFiles textures;
	};

	constexpr FaceTextureFiles Same(const char* texture)
	{
		return { texture, texture, texture, texture, texture, texture };
	}

	constexpr FaceTextureFiles TopBottomSide(const char* top, const char* bottom, const char* side)
	{
		return { top, bottom, side, side, side, side };
	}
}
