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

	std::optional<NetworkMessage> TryParsePacket(std::span<const std::byte> packetBytes)
	{
		if (packetBytes.size() < PacketHeaderByteCount) return std::nullopt;
		PacketHeader packetHeader{};
		std::memcpy(&packetHeader.magic, packetBytes.data(), sizeof(packetHeader.magic));
		std::memcpy(&packetHeader.version, packetBytes.data() + 4, sizeof(packetHeader.version));
		std::memcpy(&packetHeader.messageType, packetBytes.data() + 6, sizeof(packetHeader.messageType));
		std::memcpy(&packetHeader.payloadByteCount, packetBytes.data() + 8, sizeof(packetHeader.payloadByteCount));
		if (packetHeader.magic != ProtocolMagic || packetHeader.version != ProtocolVersion) return std::nullopt;
		if (packetBytes.size() != PacketHeaderByteCount + packetHeader.payloadByteCount) return std::nullopt;
		ByteBuffer payloadBytes(packetBytes.begin() + PacketHeaderByteCount, packetBytes.end());
		return NetworkMessage{ packetHeader.messageType, std::move(payloadBytes) };
	}
}
