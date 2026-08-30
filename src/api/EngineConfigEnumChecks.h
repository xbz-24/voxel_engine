#pragma once

#include "voxel/WorldEdits.h"

#include <cstdint>

namespace voxel::detail::config_validation
{
	[[nodiscard]] constexpr bool IsKnownPublicBlock(Block block) noexcept
	{
		return static_cast<std::uint8_t>(block) <= static_cast<std::uint8_t>(Block::DeadBush);
	}

	[[nodiscard]] constexpr bool IsKnownPublicWorldEditKind(WorldEdit::Kind kind) noexcept
	{
		switch (kind)
		{
		case WorldEdit::Kind::SetBlock:
		case WorldEdit::Kind::FillBox:
			return true;
		default:
			return false;
		}
	}
}
