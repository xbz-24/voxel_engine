#include <doctest/doctest.h>

#include "MultiplayerInboxLimitTestSupport.h"
#include "MultiplayerServer.h"

TEST_CASE("multiplayer server disconnects the producer that fills its receive inbox")
{
	const auto endpoint = ve::tests::ReserveInboxTestEndpoint();
	ve::network::MultiplayerServer server;
	REQUIRE(server.Start({ endpoint, 1 }, 1));
	auto client = ve::network::TcpSocket::Connect(endpoint);
	REQUIRE(client.has_value());
	REQUIRE(ve::network::SendNetworkMessage(*client, ve::tests::InboxTestHello()));
	REQUIRE(ve::tests::WaitForInboxState([&]
	{
		return server.Stats().activeClientCount == 1;
	}));
	REQUIRE(ve::tests::SendInboxOverflowBurst(*client));
	REQUIRE(ve::tests::WaitForInboxState([&]
	{
		return server.Stats().activeClientCount == 0;
	}));

	const auto messages = server.DrainIncomingMessages();
	CHECK(messages.size() == ve::network::MultiplayerInboxMessageCapacity);
	client->Close();
	server.Stop();
	CHECK(server.DrainIncomingMessages().empty());
}
