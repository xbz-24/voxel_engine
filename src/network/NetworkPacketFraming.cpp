#include "NetworkProtocol.h"

#include <cstddef>
#include <cstring>
#include <utility>

#include "NetworkPacketHeaderCodec.inl"

namespace ve::network
{
	ByteBuffer BuildPacket(
		NetworkMessageType messageType,
		std::span<const std::byte> payloadBytes,
		std::uint32_t sequenceNumber)
	{
		if (payloadBytes.size() > static_cast<std::size_t>(MaxPacketPayloadByteCount)) return {};
		if (!IsKnownMessageType(messageType)) return {};
		ByteBuffer packetBytes;
		const PacketHeader packetHeader{
			ProtocolMagic,
			ProtocolVersion,
			messageType,
			sequenceNumber,
			static_cast<std::uint32_t>(payloadBytes.size()),
			CalculatePayloadChecksum(payloadBytes)
		};
		packetBytes.reserve(PacketHeaderByteCount + payloadBytes.size());
		AppendHeaderField(packetBytes, packetHeader.magic);
		AppendHeaderField(packetBytes, packetHeader.version);
		AppendHeaderField(packetBytes, packetHeader.messageType);
		AppendHeaderField(packetBytes, packetHeader.sequenceNumber);
		AppendHeaderField(packetBytes, packetHeader.payloadByteCount);
		AppendHeaderField(packetBytes, packetHeader.payloadChecksum);
		packetBytes.insert(packetBytes.end(), payloadBytes.begin(), payloadBytes.end());
		return packetBytes;
	}

	std::optional<PacketHeader> TryParsePacketHeader(std::span<const std::byte> headerBytes)
	{
		if (headerBytes.size() != PacketHeaderByteCount) return std::nullopt;
		PacketHeader packetHeader{};
		std::size_t headerByteOffset = 0;
		ReadHeaderField(headerBytes, headerByteOffset, packetHeader.magic);
		ReadHeaderField(headerBytes, headerByteOffset, packetHeader.version);
		ReadHeaderField(headerBytes, headerByteOffset, packetHeader.messageType);
		ReadHeaderField(headerBytes, headerByteOffset, packetHeader.sequenceNumber);
		ReadHeaderField(headerBytes, headerByteOffset, packetHeader.payloadByteCount);
		ReadHeaderField(headerBytes, headerByteOffset, packetHeader.payloadChecksum);
		if (packetHeader.magic != ProtocolMagic || packetHeader.version != ProtocolVersion) return std::nullopt;
		if (!IsKnownMessageType(packetHeader.messageType)) return std::nullopt;
		if (packetHeader.payloadByteCount > MaxPacketPayloadByteCount) return std::nullopt;
		return packetHeader;
	}

	std::optional<NetworkMessage> TryParsePacket(std::span<const std::byte> packetBytes)
	{
		if (packetBytes.size() < PacketHeaderByteCount) return std::nullopt;
		const std::span<const std::byte> packetHeaderBytes(packetBytes.data(), PacketHeaderByteCount);
		const std::optional<PacketHeader> packetHeader = TryParsePacketHeader(packetHeaderBytes);
		if (!packetHeader.has_value()) return std::nullopt;
		const std::size_t payloadByteCount = packetHeader->payloadByteCount;
		if (packetBytes.size() - PacketHeaderByteCount != payloadByteCount) return std::nullopt;
		const std::span<const std::byte> payloadBytes(packetBytes.data() + PacketHeaderByteCount, payloadByteCount);
		if (CalculatePayloadChecksum(payloadBytes) != packetHeader->payloadChecksum) return std::nullopt;
		ByteBuffer messagePayloadBytes(payloadBytes.begin(), payloadBytes.end());
		return NetworkMessage{
			packetHeader->messageType,
			std::move(messagePayloadBytes),
			packetHeader->sequenceNumber
		};
	}
}
