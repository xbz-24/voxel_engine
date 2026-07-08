#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <memory>
#include <optional>

namespace ve::network
{
	using AddressInfoList = std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>;

	[[nodiscard]] inline addrinfo TcpStreamAddressHints(int flags = 0) noexcept
	{
		addrinfo hints{};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = flags;
		return hints;
	}

	[[nodiscard]] inline std::optional<AddressInfoList> ResolveAddressInfo(
		const char* hostName,
		const char* portText,
		const addrinfo& hints)
	{
		addrinfo* rawAddresses = nullptr;
		if (getaddrinfo(hostName, portText, &hints, &rawAddresses) != 0) return std::nullopt;
		return AddressInfoList(rawAddresses, freeaddrinfo);
	}
}
