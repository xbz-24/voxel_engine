#include "NetworkPacketIO.h"

#include <array>
#include <cstring>

namespace ve::network
{
	bool SendNetworkMessage(const TcpSocket& socket, const NetworkMessage& message)
	{
		const ByteBuffer packetBytes = BuildPacket(message.messageType, message.payloadBytes, message.sequenceNumber);
		if (packetBytes.empty()) return false;
		return socket.SendBytes(packetBytes);
	}

	std::optional<NetworkMessage> ReceiveNetworkMessage(const TcpSocket& socket)
	{
		std::array<std::byte, PacketHeaderByteCount> headerBytes{};
		if (!socket.ReceiveBytes(headerBytes)) return std::nullopt;
		const std::optional<PacketHeader> packetHeader = TryParsePacketHeader(headerBytes);
		if (!packetHeader.has_value()) return std::nullopt;
		ByteBuffer packetBytes(headerBytes.begin(), headerBytes.end());
		packetBytes.resize(PacketHeaderByteCount + packetHeader->payloadByteCount);
		std::span<std::byte> payloadBytes(packetBytes.data() + PacketHeaderByteCount, packetHeader->payloadByteCount);
		if (!socket.ReceiveBytes(payloadBytes)) return std::nullopt;
		return TryParsePacket(packetBytes);
	}
}
