#pragma once

#include "Block.h"

#include <array>

namespace ve::gameplay
{
	constexpr int HotbarSlotCount = 9;

	/**
	 * Returns the default creative hotbar block ids.
	 *
	 * @return Stable array of block ids assigned to hotbar slots.
	 */
	const std::array<ve::blocks::BlockId, HotbarSlotCount>& DefaultHotbarBlocks();

	/**
	 * Finds the hotbar slot for a block id.
	 *
	 * @param blockId Block id to search for.
	 * @return Zero-based hotbar slot index, or 0 when not found.
	 */
	int HotbarIndexFor(ve::blocks::BlockId blockId);
}

