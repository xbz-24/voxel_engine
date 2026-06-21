#pragma once

#include <array>
#include <cstddef>
#include <type_traits>
#include <vector>

namespace ve::core
{
	using Index = std::size_t;
	using ByteCount = std::size_t;

	template <typename Value>
	using DynamicArray = std::vector<Value>;

	template <typename Value, Index Count>
	using StaticArray = std::array<Value, Count>;

	/** @param value Enum or numeric value to convert. @return Value as an array index. */
	template <typename Value>
	constexpr Index ToIndex(Value value) noexcept
	{
		return static_cast<Index>(value);
	}

	/** @param value Object to move from. @return Rvalue reference to the same object. */
	template <typename Value>
	constexpr std::remove_reference_t<Value>&& Move(Value&& value) noexcept
	{
		return static_cast<std::remove_reference_t<Value>&&>(value);
	}

	/** @param value Input value. @param minimum Low bound. @param maximum High bound. @return Clamped value. */
	template <typename Value>
	constexpr Value Clamp(Value value, Value minimum, Value maximum) noexcept
	{
		return value < minimum ? minimum : (maximum < value ? maximum : value);
	}
}
