#include "NetworkTcpSocket.h"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

namespace ve::network
{
	bool TcpSocket::SendBytes(std::span<const std::byte> bytes) const
	{
		std::size_t bytesAlreadySent = 0;
		while (bytesAlreadySent < bytes.size())
		{
			const char* nextByte = reinterpret_cast<const char*>(bytes.data() + bytesAlreadySent);
			const int sentByteCount = send(static_cast<SOCKET>(_nativeSocketHandle), nextByte, static_cast<int>(bytes.size() - bytesAlreadySent), 0);
			if (sentByteCount <= 0) return false;
			bytesAlreadySent += static_cast<std::size_t>(sentByteCount);
		}
		return true;
	}

	bool TcpSocket::ReceiveBytes(std::span<std::byte> destinationBytes) const
	{
		std::size_t bytesAlreadyReceived = 0;
		while (bytesAlreadyReceived < destinationBytes.size())
		{
			char* nextByte = reinterpret_cast<char*>(destinationBytes.data() + bytesAlreadyReceived);
			const int receivedByteCount = recv(static_cast<SOCKET>(_nativeSocketHandle), nextByte, static_cast<int>(destinationBytes.size() - bytesAlreadyReceived), 0);
			if (receivedByteCount <= 0) return false;
			bytesAlreadyReceived += static_cast<std::size_t>(receivedByteCount);
		}
		return true;
	}
}
