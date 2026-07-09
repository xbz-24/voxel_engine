#include "NetworkAddressInfo.h"
#include "NetworkTcpSocket.h"
#include "NetworkSocketPlatform.h"

#include <WS2tcpip.h>

#include <string>

namespace ve::network
{
	std::optional<TcpSocket> TcpSocket::Connect(const NetworkEndpoint& remoteEndpoint)
	{
		const addrinfo addressHints = TcpStreamAddressHints();
		const std::string portText = std::to_string(remoteEndpoint.port);
		std::optional<AddressInfoList> resolvedAddresses = ResolveAddressInfo(remoteEndpoint.hostName.c_str(), portText.c_str(), addressHints);
		if (!resolvedAddresses) return std::nullopt;

		const addrinfo& firstAddress = **resolvedAddresses;
		SOCKET connectedSocket = socket(firstAddress.ai_family, firstAddress.ai_socktype, firstAddress.ai_protocol);
		if (connectedSocket != INVALID_SOCKET &&
			connect(connectedSocket, firstAddress.ai_addr, platform::SocketAddressByteCount(firstAddress.ai_addrlen)) != 0)
		{
			closesocket(connectedSocket);
			connectedSocket = INVALID_SOCKET;
		}
		if (connectedSocket == INVALID_SOCKET) return std::nullopt;
		return TcpSocket(platform::StoreNativeSocket(connectedSocket));
	}
}
