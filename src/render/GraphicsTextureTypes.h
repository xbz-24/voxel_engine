#pragma once

#include <cstdint>

namespace ve::rendering
{
	/** Backend-neutral texture identifier. Native API ids stay behind backend code. */
	struct TextureHandle
	{
		std::uint32_t value = 0;

		[[nodiscard]] constexpr bool IsValid() const noexcept { return value != 0; }
		[[nodiscard]] explicit constexpr operator bool() const noexcept { return IsValid(); }
	};

	[[nodiscard]] constexpr bool operator==(TextureHandle left, TextureHandle right) noexcept
	{
		return left.value == right.value;
	}

	[[nodiscard]] constexpr bool operator!=(TextureHandle left, TextureHandle right) noexcept
	{
		return !(left == right);
	}

	[[nodiscard]] constexpr bool operator<(TextureHandle left, TextureHandle right) noexcept
	{
		return left.value < right.value;
	}

	[[nodiscard]] constexpr bool operator==(TextureHandle left, std::uint32_t right) noexcept
	{
		return left.value == right;
	}

	[[nodiscard]] constexpr bool operator==(std::uint32_t left, TextureHandle right) noexcept
	{
		return left == right.value;
	}

	inline constexpr TextureHandle kInvalidTextureHandle{};
}
