#include <doctest/doctest.h>

#include "NetworkProtocol.h"
#include "NetworkBlockReplication.h"
#include "MultiplayerServer.h"
#include "NetworkSequenceTracker.h"
#include "NetworkSession.h"
#include "NetworkSerialization.h"
#include "World.h"
#include "TestTypeHelpers.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

#include <future>

TEST_CASE("asio tcp adapter transfers an exact byte span over loopback")
{
	const ve::network::TcpListenSettings listen_settings{
		ve::network::NetworkEndpoint{ "127.0.0.1", 0 },
		1
	};
	std::optional<ve::network::TcpSocket> listener = ve::network::TcpSocket::Listen(listen_settings);
	REQUIRE(listener.has_value());
	const std::optional<ve::network::NetworkEndpoint> local_endpoint = listener->LocalEndpoint();
	REQUIRE(local_endpoint.has_value());
	CHECK(local_endpoint->port != 0);

	std::optional<ve::network::TcpSocket> client = ve::network::TcpSocket::Connect(
		ve::network::NetworkEndpoint{ "127.0.0.1", local_endpoint->port });
	REQUIRE(client.has_value());

	const std::array<std::byte, 4> sent_bytes{ {
		std::byte{ 0x10 },
		std::byte{ 0x20 },
		std::byte{ 0x30 },
		std::byte{ 0x40 }
	} };
	std::array<std::byte, sent_bytes.size()> received_bytes{};
	std::future<bool> receive_result = std::async(std::launch::async, [&listener, &received_bytes]
	{
		std::optional<ve::network::TcpSocket> peer = listener->Accept();
		return peer && peer->ReceiveBytes(received_bytes);
	});

	const bool sent_every_byte = client->SendBytes(sent_bytes);
	if (!sent_every_byte) client->Close();
	CHECK(sent_every_byte);
	CHECK(receive_result.get());
	CHECK(received_bytes == sent_bytes);
}
