#include "NetworkTcpSocket.h"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>

namespace ve::network
{
	std::optional<TcpSocket> TcpSocket::Listen(const TcpListenSettings& listenSettings)
	{
		addrinfo addressHints{};
		addressHints.ai_family = AF_INET;
		addressHints.ai_socktype = SOCK_STREAM;
		addressHints.ai_protocol = IPPROTO_TCP;
		addressHints.ai_flags = AI_PASSIVE;
		addrinfo* resolvedAddresses = nullptr;
		const std::string portText = std::to_string(listenSettings.endpoint.port);
		const char* hostName = listenSettings.endpoint.hostName.empty() ? nullptr : listenSettings.endpoint.hostName.c_str();
		if (getaddrinfo(hostName, portText.c_str(), &addressHints, &resolvedAddresses) != 0) return std::nullopt;
		SOCKET listenSocket = socket(resolvedAddresses->ai_family, resolvedAddresses->ai_socktype, resolvedAddresses->ai_protocol);
		const bool bound = listenSocket != INVALID_SOCKET && bind(listenSocket, resolvedAddresses->ai_addr, static_cast<int>(resolvedAddresses->ai_addrlen)) == 0;
		freeaddrinfo(resolvedAddresses);
		if (!bound || listen(listenSocket, listenSettings.pendingConnectionBacklog) != 0)
		{
			if (listenSocket != INVALID_SOCKET) closesocket(listenSocket);
			return std::nullopt;
		}
		return TcpSocket(static_cast<std::uintptr_t>(listenSocket));
	}

	std::optional<TcpSocket> TcpSocket::Accept() const
	{
		const SOCKET acceptedSocket = accept(static_cast<SOCKET>(_nativeSocketHandle), nullptr, nullptr);
		if (acceptedSocket == INVALID_SOCKET) return std::nullopt;
		return TcpSocket(static_cast<std::uintptr_t>(acceptedSocket));
	}
}
