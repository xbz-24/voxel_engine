#include <doctest/doctest.h>

#include "MultiplayerServer.h"
#include "NetworkPacketIO.h"
#include "NetworkSerialization.h"

#include <chrono>
#include <cstddef>
#include <string>
#include <thread>

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

	bool WaitForStats(
		ve::network::MultiplayerServer& server, std::size_t active, std::size_t retained)
	{
		const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
		const ve::network::NetworkMessage probe{ ve::network::NetworkMessageType::Ping, {} };
		do
		{
			static_cast<void>(server.Broadcast(probe));
			const auto stats = server.Stats();
			if (stats.activeClientCount == active &&
				stats.retainedClientWorkerCount == retained) return true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		} while (std::chrono::steady_clock::now() < deadline);
		return false;
	}

	bool CompleteClientCycle(ve::network::MultiplayerServer& server,
		const ve::network::NetworkEndpoint& endpoint)
	{
		auto client = ve::network::TcpSocket::Connect(endpoint);
		if (!client) return false;
		const ve::network::NetworkMessage hello{
			ve::network::NetworkMessageType::ClientHello,
			ve::network::SerializeClientHello("churn-test"), 1 };
		if (!ve::network::SendNetworkMessage(*client, hello) ||
			!WaitForStats(server, 1U, 1U)) return false;
		client->Close();
		return WaitForStats(server, 0U, 0U);
	}

	bool WaitForInbound(ve::network::MultiplayerServer& server)
	{
		const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
		while (std::chrono::steady_clock::now() < deadline)
		{
			if (!server.DrainIncomingMessages().empty()) return true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return false;
	}
}

TEST_CASE("multiplayer server reaps connection churn and remains functional after restart")
{
	const auto endpoint = ReserveEndpoint();
	ve::network::MultiplayerServer server;
	REQUIRE(server.Start({ endpoint, 4 }, 1));
	for (std::size_t cycle = 0; cycle < 64U; ++cycle)
	{
		CAPTURE(cycle);
		REQUIRE(CompleteClientCycle(server, endpoint));
	}
	server.Stop();

	REQUIRE(server.Start({ endpoint, 4 }, 1));
	auto client = ve::network::TcpSocket::Connect(endpoint);
	REQUIRE(client.has_value());
	const ve::network::NetworkMessage hello{
		ve::network::NetworkMessageType::ClientHello,
		ve::network::SerializeClientHello("restart-test"), 1 };
	REQUIRE(ve::network::SendNetworkMessage(*client, hello));
	const ve::network::NetworkMessage mutation{
		ve::network::NetworkMessageType::BlockMutation,
		ve::network::SerializeBlockMutation({ 1, 2, 3, 4, 5, 6 }), 2 };
	REQUIRE(ve::network::SendNetworkMessage(*client, mutation));
	REQUIRE(WaitForInbound(server));
	REQUIRE(WaitForStats(server, 1U, 1U));
	client->Close();
	REQUIRE(WaitForStats(server, 0U, 0U));
	server.Stop();
}
