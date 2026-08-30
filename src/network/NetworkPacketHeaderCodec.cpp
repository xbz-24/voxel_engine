#include "NetworkProtocol.h"

#include "CoreTypes.h"
#include "NetworkByteCodec.h"
#include "NetworkPacketCodecDetail.h"

namespace
{
	constexpr std::uint32_t SerializedPacketHeaderByteCount =
		sizeof(std::uint32_t) + sizeof(std::uint16_t) + sizeof(ve::network::NetworkMessageType) +
		sizeof(std::uint32_t) + sizeof(std::uint32_t) + sizeof(std::uint32_t);
	static_assert(ve::network::PacketHeaderByteCount == SerializedPacketHeaderByteCount);
}

namespace ve::network
{
	ByteBuffer BuildPacket(
		NetworkMessageType messageType,
		std::span<const std::byte> payloadBytes,
		std::uint32_t sequenceNumber)
	{
		if (payloadBytes.size() > ve::core::ToIndex(MaxPacketPayloadByteCount)) return {};
		if (!detail::IsKnownMessageType(messageType)) return {};

		const PacketHeader packetHeader{
			ProtocolMagic,
			ProtocolVersion,
			messageType,
			sequenceNumber,
			ve::core::ToU32(payloadBytes.size()),
			detail::CalculatePayloadChecksum(payloadBytes)
		};
		ByteBuffer packetBytes;
		packetBytes.reserve(PacketHeaderByteCount + payloadBytes.size());
		AppendSerializedValue(packetBytes, packetHeader.magic);
		AppendSerializedValue(packetBytes, packetHeader.version);
		AppendSerializedValue(packetBytes, packetHeader.messageType);
		AppendSerializedValue(packetBytes, packetHeader.sequenceNumber);
		AppendSerializedValue(packetBytes, packetHeader.payloadByteCount);
		AppendSerializedValue(packetBytes, packetHeader.payloadChecksum);
		packetBytes.insert(packetBytes.end(), payloadBytes.begin(), payloadBytes.end());
		return packetBytes;
	}
}
