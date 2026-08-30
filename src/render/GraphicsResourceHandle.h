#pragma once

#include <cstdint>

namespace ve::rendering
{
	struct GraphicsResourceHandle
	{
		std::uint32_t index = UINT32_MAX;
		std::uint32_t generation = 0;

		/** @return True when the handle points at a possible slot. */
		[[nodiscard]] bool IsValid() const noexcept { return index != UINT32_MAX; }

		/** @return True when both handles refer to the same generation. */
		[[nodiscard]] bool operator==(const GraphicsResourceHandle& other) const noexcept = default;
	};
}
