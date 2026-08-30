#pragma once

#include "NetworkTypes.h"

#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <type_traits>

namespace ve::network
{
	template <typename Value>
	[[nodiscard]] std::span<const std::byte> SerializedValueBytes(const Value& value) noexcept
	{
		static_assert(std::is_trivially_copyable_v<Value>);
		const std::span<const Value> value_span{ &value, 1U };
		return std::as_bytes(value_span);
	}

	template <typename Value>
	void AppendSerializedValue(ByteBuffer& destination_bytes, const Value& value)
	{
		const std::span<const std::byte> source_bytes = SerializedValueBytes(value);
		destination_bytes.insert(destination_bytes.end(), source_bytes.begin(), source_bytes.end());
	}

	template <typename Value>
	bool ReadSerializedValue(
		std::span<const std::byte> source_bytes,
		std::size_t& read_offset,
		Value& output) noexcept
	{
		static_assert(std::is_trivially_copyable_v<Value>);
		if (read_offset > source_bytes.size()) return false;
		if (source_bytes.size() - read_offset < sizeof(Value)) return false;
		std::memcpy(&output, source_bytes.data() + read_offset, sizeof(Value));
		read_offset += sizeof(Value);
		return true;
	}

	[[nodiscard]] inline std::optional<std::span<const std::byte>> ReadByteSpan(
		std::span<const std::byte> source_bytes,
		std::size_t& read_offset,
		std::size_t byte_count) noexcept
	{
		if (read_offset > source_bytes.size()) return std::nullopt;
		if (source_bytes.size() - read_offset < byte_count) return std::nullopt;
		const std::span<const std::byte> bytes{ source_bytes.data() + read_offset, byte_count };
		read_offset += byte_count;
		return bytes;
	}

	[[nodiscard]] inline std::string ByteSpanToString(std::span<const std::byte> source_bytes)
	{
		std::string value(source_bytes.size(), '\0');
		std::memcpy(value.data(), source_bytes.data(), source_bytes.size());
		return value;
	}

}
