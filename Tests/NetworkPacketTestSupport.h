#pragma once

#include "NetworkProtocol.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

template <typename Value>
inline void WritePacketHeaderField(
	std::array<std::byte, ve::network::PacketHeaderByteCount>& bytes,
	std::size_t& offset,
	const Value& value)
{
	std::memcpy(bytes.data() + offset, &value, sizeof(Value));
	offset += sizeof(Value);
}

[[nodiscard]] inline std::array<std::byte, ve::network::PacketHeaderByteCount>
BuildPacketHeaderForTest(
	ve::network::NetworkMessageType message_type,
	std::uint32_t payload_byte_count)
{
	std::array<std::byte, ve::network::PacketHeaderByteCount> bytes{};
	std::size_t offset = 0;
	const std::uint32_t magic = ve::network::ProtocolMagic;
	const std::uint16_t version = ve::network::ProtocolVersion;
	const std::uint32_t sequence_number = 0;
	const std::uint32_t payload_checksum = 0;
	WritePacketHeaderField(bytes, offset, magic);
	WritePacketHeaderField(bytes, offset, version);
	WritePacketHeaderField(bytes, offset, message_type);
	WritePacketHeaderField(bytes, offset, sequence_number);
	WritePacketHeaderField(bytes, offset, payload_byte_count);
	WritePacketHeaderField(bytes, offset, payload_checksum);
	return bytes;
}
