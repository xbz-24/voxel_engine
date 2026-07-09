#include "NetworkAddressInfo.h"
#include "NetworkTcpSocket.h"
#include "NetworkSocketPlatform.h"

#include <WS2tcpip.h>

#include <string>

namespace ve::network
{
	std::optional<TcpSocket> TcpSocket::Listen(const TcpListenSettings& listenSettings)
	{
		const addrinfo addressHints = TcpStreamAddressHints(AI_PASSIVE);
		const std::string portText = std::to_string(listenSettings.endpoint.port);
		const char* hostName = listenSettings.endpoint.hostName.empty() ? nullptr : listenSettings.endpoint.hostName.c_str();
		std::optional<AddressInfoList> resolvedAddresses = ResolveAddressInfo(hostName, portText.c_str(), addressHints);
		if (!resolvedAddresses) return std::nullopt;

		const addrinfo& firstAddress = **resolvedAddresses;
		SOCKET listenSocket = socket(firstAddress.ai_family, firstAddress.ai_socktype, firstAddress.ai_protocol);
		const bool bound = listenSocket != INVALID_SOCKET &&
			bind(listenSocket, firstAddress.ai_addr, platform::SocketAddressByteCount(firstAddress.ai_addrlen)) == 0;
		if (!bound || listen(listenSocket, listenSettings.pendingConnectionBacklog) != 0)
		{
			if (listenSocket != INVALID_SOCKET) closesocket(listenSocket);
			return std::nullopt;
		}
		return TcpSocket(platform::StoreNativeSocket(listenSocket));
	}

	std::optional<TcpSocket> TcpSocket::Accept() const
	{
		const SOCKET acceptedSocket = accept(platform::ToNativeSocket(_nativeSocketHandle), nullptr, nullptr);
		if (acceptedSocket == INVALID_SOCKET) return std::nullopt;
		return TcpSocket(platform::StoreNativeSocket(acceptedSocket));
	}
}
