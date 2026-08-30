#pragma once

#include "NetworkByteCodec.h"

#include <cstddef>
#include <optional>
#include <span>

namespace ve::network::detail
{
	class PayloadWriter
	{
	public:
		template <typename Value>
		void Write(const Value& value)
		{
			AppendSerializedValue(_payloadBytes, value);
		}

		void WriteBytes(std::span<const std::byte> sourceBytes);

		[[nodiscard]] ByteBuffer Finish() &&;

	private:
		ByteBuffer _payloadBytes;
	};

	class PayloadReader
	{
	public:
		explicit PayloadReader(std::span<const std::byte> serializedPayloadBytes) noexcept;

		template <typename Value>
		bool Read(Value& output)
		{
			return ReadSerializedValue(_serializedPayloadBytes, _readOffset, output);
		}

		[[nodiscard]] std::optional<std::span<const std::byte>> ReadBytes(std::size_t byteCount);
		[[nodiscard]] bool IsFinished() const noexcept;

	private:
		std::span<const std::byte> _serializedPayloadBytes;
		std::size_t _readOffset = 0;
	};
}
