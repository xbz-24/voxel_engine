#pragma once

#include "CoreTypes.h"

namespace ve::tests
{
	template <typename Enum>
	[[nodiscard]] constexpr Enum InvalidEnumValue(int value = 255) noexcept
	{
		return ve::core::NumericCast<Enum>(value);
	}
}
