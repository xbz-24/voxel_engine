#pragma once

#include "Block.h"

namespace ve::blocks
{
	struct SolidBlockColor
	{
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
	};

	struct SolidBlockPaletteEntry
	{
		BlockId block = BlockId::Air;
		SolidBlockColor color{};
	};
}
