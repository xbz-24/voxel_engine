#include <doctest/doctest.h>

#include "MultiplayerServer.h"
#include "NetworkPacketIO.h"
#include "NetworkSerialization.h"

#include <chrono>
#include <future>
#include <optional>
#include <thread>
#include <utility>

namespace
{
	ve::network::NetworkEndpoint ReserveEndpoint()
	{
		auto listener = ve::network::TcpSocket::Listen({ { "127.0.0.1", 0 }, 1 });
		REQUIRE(listener.has_value());
		const auto endpoint = listener->LocalEndpoint();
		REQUIRE(endpoint.has_value());
		listener->Close();
		return { "127.0.0.1", endpoint->port };
	}

	bool WaitForInboundMessage(ve::network::MultiplayerServer& server)
	{
		const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
		do
		{
			if (!server.DrainIncomingMessages().empty()) return true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		} while (std::chrono::steady_clock::now() < deadline);
		return false;
	}

	template<typename Recovery>
	bool StopsWithoutRecovery(
		ve::network::MultiplayerServer& server, Recovery&& recovery)
	{
		std::future<void> stopped = std::async(std::launch::async, [&server]
		{
			server.Stop();
		});
		const bool completed = stopped.wait_for(std::chrono::seconds(2)) ==
			std::future_status::ready;
		if (!completed) std::forward<Recovery>(recovery)();
		if (stopped.wait_for(std::chrono::seconds(2)) != std::future_status::ready)
		{
			return false;
		}
		stopped.get();
		return completed;
	}
}

TEST_CASE("multiplayer server stop cancels a pending accept before close")
{
	ve::network::MultiplayerServer server;
	const auto endpoint = ReserveEndpoint();
	REQUIRE(server.Start({ endpoint, 1 }, 1));
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	std::optional<ve::network::TcpSocket> rescue_client;
	CHECK(StopsWithoutRecovery(server, [&]
	{
		rescue_client = ve::network::TcpSocket::Connect(endpoint);
	}));
}

TEST_CASE("multiplayer server stop quiesces a blocked client receive before close")
{
	const auto endpoint = ReserveEndpoint();
	ve::network::MultiplayerServer server;
	REQUIRE(server.Start({ endpoint, 1 }, 1));
	auto client = ve::network::TcpSocket::Connect(endpoint);
	REQUIRE(client.has_value());
	const ve::network::NetworkMessage hello{
		ve::network::NetworkMessageType::ClientHello,
		ve::network::SerializeClientHello("quiescence-test"),
		1
	};
	REQUIRE(ve::network::SendNetworkMessage(*client, hello));
	const ve::network::NetworkMessage mutation{
		ve::network::NetworkMessageType::BlockMutation,
		ve::network::SerializeBlockMutation({ 1, 2, 3, 4, 5, 6 }),
		2
	};
	REQUIRE(ve::network::SendNetworkMessage(*client, mutation));
	REQUIRE(WaitForInboundMessage(server));
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	CHECK(StopsWithoutRecovery(server, [&] { client->Close(); }));
	client->Close();
}
