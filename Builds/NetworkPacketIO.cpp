#include "NetworkPacketIO.h"

#include <array>
#include <cstring>

namespace ve::network
{
	bool SendNetworkMessage(const TcpSocket& socket, const NetworkMessage& message)
	{
		const ByteBuffer packetBytes = BuildPacket(message.messageType, message.payloadBytes);
		return socket.SendBytes(packetBytes);
	}

	std::optional<NetworkMessage> ReceiveNetworkMessage(const TcpSocket& socket)
	{
		std::array<std::byte, PacketHeaderByteCount> headerBytes{};
		if (!socket.ReceiveBytes(headerBytes)) return std::nullopt;
		std::uint32_t payloadByteCount = 0;
		std::memcpy(&payloadByteCount, headerBytes.data() + 8, sizeof(payloadByteCount));
		ByteBuffer packetBytes(headerBytes.begin(), headerBytes.end());
		packetBytes.resize(PacketHeaderByteCount + payloadByteCount);
		std::span<std::byte> payloadBytes(packetBytes.data() + PacketHeaderByteCount, payloadByteCount);
		if (!socket.ReceiveBytes(payloadBytes)) return std::nullopt;
		return TryParsePacket(packetBytes);
	}
}
