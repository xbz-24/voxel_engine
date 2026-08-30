#pragma once

#include <cstdint>

namespace voxel::detail
{
	using WideBlockCoordinate = std::int64_t;

	[[nodiscard]] constexpr WideBlockCoordinate WidenBlockCoordinate(int value) noexcept
	{
		return static_cast<WideBlockCoordinate>(value);
	}

	[[nodiscard]] bool TryBlockCoordinate(
		WideBlockCoordinate value,
		int& destination) noexcept;
	[[nodiscard]] bool TryCenteredBlockSpan(
		int center,
		int radius,
		int& first,
		int& second) noexcept;
}
