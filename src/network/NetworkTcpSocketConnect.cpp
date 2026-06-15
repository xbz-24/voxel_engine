#include "NetworkTcpSocket.h"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>

namespace ve::network
{
	std::optional<TcpSocket> TcpSocket::Connect(const NetworkEndpoint& remoteEndpoint)
	{
		addrinfo addressHints{};
		addressHints.ai_family = AF_INET;
		addressHints.ai_socktype = SOCK_STREAM;
		addressHints.ai_protocol = IPPROTO_TCP;
		addrinfo* resolvedAddresses = nullptr;
		const std::string portText = std::to_string(remoteEndpoint.port);
		if (getaddrinfo(remoteEndpoint.hostName.c_str(), portText.c_str(), &addressHints, &resolvedAddresses) != 0) return std::nullopt;
		SOCKET connectedSocket = socket(resolvedAddresses->ai_family, resolvedAddresses->ai_socktype, resolvedAddresses->ai_protocol);
		if (connectedSocket != INVALID_SOCKET && connect(connectedSocket, resolvedAddresses->ai_addr, static_cast<int>(resolvedAddresses->ai_addrlen)) != 0)
		{
			closesocket(connectedSocket);
			connectedSocket = INVALID_SOCKET;
		}
		freeaddrinfo(resolvedAddresses);
		if (connectedSocket == INVALID_SOCKET) return std::nullopt;
		return TcpSocket(static_cast<std::uintptr_t>(connectedSocket));
	}
}
