#include "NetworkProtocol.h"

#include "NetworkByteCodec.h"
#include "NetworkPacketCodecDetail.h"

#include <utility>

namespace ve::network
{
	std::optional<PacketHeader> TryParsePacketHeader(std::span<const std::byte> headerBytes)
	{
		if (headerBytes.size() != PacketHeaderByteCount) return std::nullopt;
		PacketHeader header{};
		std::size_t offset = 0;
		if (!ReadSerializedValue(headerBytes, offset, header.magic)) return std::nullopt;
		if (!ReadSerializedValue(headerBytes, offset, header.version)) return std::nullopt;
		if (!ReadSerializedValue(headerBytes, offset, header.messageType)) return std::nullopt;
		if (!ReadSerializedValue(headerBytes, offset, header.sequenceNumber)) return std::nullopt;
		if (!ReadSerializedValue(headerBytes, offset, header.payloadByteCount)) return std::nullopt;
		if (!ReadSerializedValue(headerBytes, offset, header.payloadChecksum)) return std::nullopt;
		if (header.magic != ProtocolMagic || header.version != ProtocolVersion) return std::nullopt;
		if (!detail::IsKnownMessageType(header.messageType)) return std::nullopt;
		if (header.payloadByteCount > MaxPacketPayloadByteCount) return std::nullopt;
		return header;
	}

	std::optional<NetworkMessage> TryParsePacket(std::span<const std::byte> packetBytes)
	{
		if (packetBytes.size() < PacketHeaderByteCount) return std::nullopt;
		const std::span<const std::byte> headerBytes(packetBytes.data(), PacketHeaderByteCount);
		const std::optional<PacketHeader> header = TryParsePacketHeader(headerBytes);
		if (!header) return std::nullopt;
		const std::size_t payloadByteCount = header->payloadByteCount;
		if (packetBytes.size() - PacketHeaderByteCount != payloadByteCount) return std::nullopt;
		const std::span<const std::byte> payloadBytes(
			packetBytes.data() + PacketHeaderByteCount,
			payloadByteCount);
		if (detail::CalculatePayloadChecksum(payloadBytes) != header->payloadChecksum) return std::nullopt;
		return NetworkMessage{
			header->messageType,
			ByteBuffer(payloadBytes.begin(), payloadBytes.end()),
			header->sequenceNumber
		};
	}
}
