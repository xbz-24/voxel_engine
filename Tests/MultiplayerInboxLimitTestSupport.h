#pragma once

#include <doctest/doctest.h>

#include "MultiplayerInboxLimits.h"
#include "NetworkPacketIO.h"
#include "NetworkSerialization.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <thread>

namespace ve::tests
{
	inline std::optional<ve::network::TcpSocket> OpenInboxTestListener()
	{
		return ve::network::TcpSocket::Listen({ { "127.0.0.1", 0 }, 1 });
	}

	inline ve::network::NetworkEndpoint InboxTestEndpoint(
		const ve::network::TcpSocket& listener)
	{
		const auto endpoint = listener.LocalEndpoint();
		REQUIRE(endpoint.has_value());
		return { "127.0.0.1", endpoint->port };
	}

	inline ve::network::NetworkEndpoint ReserveInboxTestEndpoint()
	{
		auto listener = OpenInboxTestListener();
		REQUIRE(listener.has_value());
		const auto endpoint = InboxTestEndpoint(*listener);
		listener->Close();
		return endpoint;
	}

	template <typename Predicate>
	bool WaitForInboxState(Predicate&& predicate)
	{
		const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
		do
		{
			if (predicate()) return true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		} while (std::chrono::steady_clock::now() < deadline);
		return predicate();
	}

	inline bool SendInboxOverflowBurst(const ve::network::TcpSocket& socket)
	{
		for (std::size_t index = 0;
			index <= ve::network::MultiplayerInboxMessageCapacity;
			++index)
		{
			const ve::network::NetworkMessage ping{
				ve::network::NetworkMessageType::Ping,
				{},
				static_cast<std::uint32_t>(index + 1)
			};
			if (!ve::network::SendNetworkMessage(socket, ping)) return false;
		}
		return true;
	}

	inline ve::network::NetworkMessage InboxTestHello()
	{
		return {
			ve::network::NetworkMessageType::ClientHello,
			ve::network::SerializeClientHello("inbox-limit-test"),
			1
		};
	}
}
