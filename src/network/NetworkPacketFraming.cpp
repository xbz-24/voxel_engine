#include "NetworkProtocol.h"

#include <cstring>

namespace
{
	/// Appends raw bytes for one packet header field.
	template <typename Value>
	void AppendField(ve::network::ByteBuffer& bytes, const Value& value)
	{
		const auto* firstByte = reinterpret_cast<const std::byte*>(&value);
		bytes.insert(bytes.end(), firstByte, firstByte + sizeof(Value));
	}
}

namespace ve::network
{
	ByteBuffer BuildPacket(NetworkMessageType messageType, std::span<const std::byte> payloadBytes)
	{
		if (payloadBytes.size() > static_cast<std::size_t>(MaxPacketPayloadByteCount)) return {};
		ByteBuffer packetBytes;
		const PacketHeader packetHeader{ ProtocolMagic, ProtocolVersion, messageType, static_cast<std::uint32_t>(payloadBytes.size()) };
		packetBytes.reserve(PacketHeaderByteCount + payloadBytes.size());
		AppendField(packetBytes, packetHeader.magic);
		AppendField(packetBytes, packetHeader.version);
		AppendField(packetBytes, packetHeader.messageType);
		AppendField(packetBytes, packetHeader.payloadByteCount);
		packetBytes.insert(packetBytes.end(), payloadBytes.begin(), payloadBytes.end());
		return packetBytes;
	}

	std::optional<PacketHeader> TryParsePacketHeader(std::span<const std::byte> headerBytes)
	{
		if (headerBytes.size() != PacketHeaderByteCount) return std::nullopt;
		PacketHeader packetHeader{};
		std::memcpy(&packetHeader.magic, headerBytes.data(), sizeof(packetHeader.magic));
		std::memcpy(&packetHeader.version, headerBytes.data() + 4, sizeof(packetHeader.version));
		std::memcpy(&packetHeader.messageType, headerBytes.data() + 6, sizeof(packetHeader.messageType));
		std::memcpy(&packetHeader.payloadByteCount, headerBytes.data() + 8, sizeof(packetHeader.payloadByteCount));
		if (packetHeader.magic != ProtocolMagic || packetHeader.version != ProtocolVersion) return std::nullopt;
		if (packetHeader.payloadByteCount > MaxPacketPayloadByteCount) return std::nullopt;
		return packetHeader;
	}

	std::optional<NetworkMessage> TryParsePacket(std::span<const std::byte> packetBytes)
	{
		if (packetBytes.size() < PacketHeaderByteCount) return std::nullopt;
		const std::span<const std::byte> header_bytes(packetBytes.data(), PacketHeaderByteCount);
		const std::optional<PacketHeader> packet_header = TryParsePacketHeader(header_bytes);
		if (!packet_header.has_value()) return std::nullopt;
		const std::size_t payload_byte_count = packet_header->payloadByteCount;
		if (packetBytes.size() - PacketHeaderByteCount != payload_byte_count) return std::nullopt;
		ByteBuffer payloadBytes(packetBytes.begin() + PacketHeaderByteCount, packetBytes.end());
		return NetworkMessage{ packet_header->messageType, std::move(payloadBytes) };
	}
}
