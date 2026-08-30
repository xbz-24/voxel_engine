#pragma once

#include <cstddef>

namespace ve::network
{
	/** Maximum decoded messages retained by a multiplayer receive queue. */
	inline constexpr std::size_t MultiplayerInboxMessageCapacity = 512;
}
