#include "NetworkPayloadCodec.h"

#include <utility>

namespace ve::network::detail
{
	void PayloadWriter::WriteBytes(std::span<const std::byte> sourceBytes)
	{
		_payloadBytes.insert(_payloadBytes.end(), sourceBytes.begin(), sourceBytes.end());
	}

	ByteBuffer PayloadWriter::Finish() &&
	{
		return std::move(_payloadBytes);
	}

	PayloadReader::PayloadReader(std::span<const std::byte> serializedPayloadBytes) noexcept
		: _serializedPayloadBytes(serializedPayloadBytes)
	{
	}

	std::optional<std::span<const std::byte>> PayloadReader::ReadBytes(std::size_t byteCount)
	{
		return ReadByteSpan(_serializedPayloadBytes, _readOffset, byteCount);
	}

	bool PayloadReader::IsFinished() const noexcept
	{
		return _readOffset == _serializedPayloadBytes.size();
	}
}
