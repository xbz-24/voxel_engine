#include "NetworkPacketIO.h"

#include <array>
#include <chrono>
#include <cstring>

namespace ve::network
{
	namespace
	{
		constexpr std::chrono::milliseconds NetworkFrameWriteTimeout{ 250 };
	}

	bool SendNetworkMessage(const TcpSocket& socket, const NetworkMessage& message)
	{
		return SendNetworkMessage(socket, message, std::stop_token{});
	}

	bool SendNetworkMessage(
		const TcpSocket& socket, const NetworkMessage& message, std::stop_token stop_token)
	{
		const ByteBuffer packetBytes = BuildPacket(message.messageType, message.payloadBytes, message.sequenceNumber);
		if (packetBytes.empty()) return false;
		if (socket.SendBytes(
			packetBytes, stop_token,
			std::chrono::steady_clock::now() + NetworkFrameWriteTimeout))
		{
			return true;
		}
		socket.Shutdown();
		return false;
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
