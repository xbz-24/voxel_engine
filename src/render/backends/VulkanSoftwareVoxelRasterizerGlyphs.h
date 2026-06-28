#pragma once

#include <array>
#include <cstdint>

namespace ve::rendering
{
	[[nodiscard]] std::array<std::uint8_t, 7> GlyphFor(char value) noexcept;
}
