#include "NetworkTcpSocket.h"

#include "NetworkByteCodec.h"
#include "NetworkSocketPlatform.h"

namespace ve::network
{
	bool TcpSocket::SendBytes(std::span<const std::byte> bytes) const
	{
		std::size_t bytesAlreadySent = 0;
		while (bytesAlreadySent < bytes.size())
		{
			const char* nextByte = SocketSendCursor(bytes, bytesAlreadySent);
			const int sentByteCount = send(
				platform::ToNativeSocket(_nativeSocketHandle),
				nextByte,
				platform::SocketTransferByteCount(bytes.size() - bytesAlreadySent),
				0);
			if (sentByteCount <= 0) return false;
			bytesAlreadySent += platform::TransferredByteCount(sentByteCount);
		}
		return true;
	}

	bool TcpSocket::ReceiveBytes(std::span<std::byte> destinationBytes) const
	{
		std::size_t bytesAlreadyReceived = 0;
		while (bytesAlreadyReceived < destinationBytes.size())
		{
			char* nextByte = SocketReceiveCursor(destinationBytes, bytesAlreadyReceived);
			const int receivedByteCount = recv(
				platform::ToNativeSocket(_nativeSocketHandle),
				nextByte,
				platform::SocketTransferByteCount(destinationBytes.size() - bytesAlreadyReceived),
				0);
			if (receivedByteCount <= 0) return false;
			bytesAlreadyReceived += platform::TransferredByteCount(receivedByteCount);
		}
		return true;
	}
}
