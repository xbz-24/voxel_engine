#include <doctest/doctest.h>

#include "MultiplayerServer.h"
#include "NetworkPacketIO.h"
#include "NetworkSerialization.h"

#include <chrono>
#include <optional>
#include <thread>

namespace
{
	ve::network::NetworkEndpoint ReserveLoopbackEndpoint()
	{
		auto reservation = ve::network::TcpSocket::Listen({ { "127.0.0.1", 0 }, 1 });
		REQUIRE(reservation.has_value());
		const std::optional<ve::network::NetworkEndpoint> endpoint = reservation->LocalEndpoint();
		REQUIRE(endpoint.has_value());
		reservation->Close();
		return { "127.0.0.1", endpoint->port };
	}

	bool WaitForRecipientCount(
		ve::network::MultiplayerServer& server,
		std::size_t expected_count)
	{
		const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
		const ve::network::NetworkMessage ping{ ve::network::NetworkMessageType::Ping, {} };
		do
		{
			if (server.Broadcast(ping) == expected_count) return true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		} while (std::chrono::steady_clock::now() < deadline);
		return false;
	}
}

TEST_CASE("multiplayer server stop discards messages from the closed session")
{
	const ve::network::NetworkEndpoint endpoint = ReserveLoopbackEndpoint();
	ve::network::MultiplayerServer server;
	REQUIRE(server.Start({ endpoint, 1 }, 1));
	auto client = ve::network::TcpSocket::Connect(endpoint);
	REQUIRE(client.has_value());

	const ve::network::NetworkMessage hello{
		ve::network::NetworkMessageType::ClientHello,
		ve::network::SerializeClientHello("lifecycle-test"),
		1
	};
	REQUIRE(ve::network::SendNetworkMessage(*client, hello));
	REQUIRE(WaitForRecipientCount(server, 1));
	const ve::network::NetworkMessage mutation{
		ve::network::NetworkMessageType::BlockMutation,
		ve::network::SerializeBlockMutation({ 1, 2, 3, 4, 5, 6 }),
		2
	};
	REQUIRE(ve::network::SendNetworkMessage(*client, mutation));
	client->Close();
	REQUIRE(WaitForRecipientCount(server, 0));

	server.Stop();
	CHECK(server.DrainIncomingMessages().empty());
}
