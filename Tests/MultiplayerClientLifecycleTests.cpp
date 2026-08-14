#include <doctest/doctest.h>

#include "MultiplayerClient.h"
#include "NetworkPacketIO.h"
#include "NetworkSerialization.h"

#include <chrono>
#include <memory>
#include <optional>
#include <thread>

namespace
{
	std::optional<ve::network::TcpSocket> OpenLoopbackListener()
	{
		return ve::network::TcpSocket::Listen({ { "127.0.0.1", 0 }, 1 });
	}

	ve::network::NetworkEndpoint ListenerEndpoint(const ve::network::TcpSocket& listener)
	{
		const std::optional<ve::network::NetworkEndpoint> endpoint = listener.LocalEndpoint();
		REQUIRE(endpoint.has_value());
		return { "127.0.0.1", endpoint->port };
	}

	bool WaitUntilDisconnected(const ve::network::MultiplayerClient& client)
	{
		const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
		while (client.IsConnected() && std::chrono::steady_clock::now() < deadline)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return !client.IsConnected();
	}
}

TEST_CASE("multiplayer client disconnect discards messages from the closed session")
{
	auto listener = OpenLoopbackListener();
	REQUIRE(listener.has_value());
	ve::network::MultiplayerClient client;
	REQUIRE(client.Connect(ListenerEndpoint(*listener), "lifecycle-test"));
	auto peer = listener->Accept();
	REQUIRE(peer.has_value());
	REQUIRE(ve::network::ReceiveNetworkMessage(*peer).has_value());

	const ve::network::BlockMutationPayload payload{ 1, 2, 3, 4, 5, 6 };
	const ve::network::NetworkMessage message{
		ve::network::NetworkMessageType::BlockMutation,
		ve::network::SerializeBlockMutation(payload),
		1
	};
	REQUIRE(ve::network::SendNetworkMessage(*peer, message));
	peer->Close();
	REQUIRE(WaitUntilDisconnected(client));

	client.Disconnect();
	CHECK(client.DrainIncomingMessages().empty());
}

TEST_CASE("multiplayer client destruction joins a blocked receive worker")
{
	auto listener = OpenLoopbackListener();
	REQUIRE(listener.has_value());
	const ve::network::NetworkEndpoint endpoint = ListenerEndpoint(*listener);
	for (int attempt = 0; attempt < 16; ++attempt)
	{
		auto client = std::make_unique<ve::network::MultiplayerClient>();
		REQUIRE(client->Connect(endpoint, "lifecycle-test"));
		auto peer = listener->Accept();
		REQUIRE(peer.has_value());
		REQUIRE(ve::network::ReceiveNetworkMessage(*peer).has_value());
		client.reset();
		CHECK_FALSE(ve::network::ReceiveNetworkMessage(*peer).has_value());
	}
}
