#pragma once


#include <array>
#include <cstddef>
#include <cstdint>
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

	/** @param value Numeric or enum value to convert. @return Value converted to the requested target type. */
	template <typename Target, typename Value>
	constexpr Target NumericCast(Value value) noexcept
	{
		return static_cast<Target>(value);
	}

	/** @param value Numeric value to convert. @return Value as an unsigned 8-bit integer. */
	template <typename Value>
	constexpr std::uint8_t ToU8(Value value) noexcept
	{
		return static_cast<std::uint8_t>(value);
	}

	/** @param value Numeric or character value to convert. @return Value as an unsigned character. */
	template <typename Value>
	constexpr unsigned char ToUnsignedChar(Value value) noexcept
	{
		return static_cast<unsigned char>(value);
	}

	/** @param value Numeric or character value to convert. @return Value as a character. */
	template <typename Value>
	constexpr char ToChar(Value value) noexcept
	{
		return static_cast<char>(value);
	}

	/** @param value Numeric value to convert. @return Value as a signed int. */
	template <typename Value>
	constexpr int ToInt(Value value) noexcept
	{
		return static_cast<int>(value);
	}

	/** @param value Numeric value to convert. @return Value as an unsigned 32-bit integer. */
	template <typename Value>
	constexpr std::uint32_t ToU32(Value value) noexcept
	{
		return static_cast<std::uint32_t>(value);
	}

	/** @param value Numeric value to convert. @return Value as an unsigned 64-bit integer. */
	template <typename Value>
	constexpr std::uint64_t ToU64(Value value) noexcept
	{
		return static_cast<std::uint64_t>(value);
	}

	/** @param value Numeric value to convert. @return Value as a pointer/iterator difference. */
	template <typename Value>
	constexpr std::ptrdiff_t ToPtrdiff(Value value) noexcept
	{
		return static_cast<std::ptrdiff_t>(value);
	}

	/** @param value Numeric value to convert. @return Value as a 32-bit float. */
	template <typename Value>
	constexpr float ToFloat(Value value) noexcept
	{
		return static_cast<float>(value);
	}

	/** @param value Numeric value to convert. @return Value as a 64-bit float. */
	template <typename Value>
	constexpr double ToDouble(Value value) noexcept
	{
		return static_cast<double>(value);
	}

}
