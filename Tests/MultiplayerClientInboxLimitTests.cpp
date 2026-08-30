#include <doctest/doctest.h>

#include "MultiplayerClient.h"
#include "MultiplayerInboxLimitTestSupport.h"

TEST_CASE("multiplayer client disconnects when its receive inbox is full")
{
	auto listener = ve::tests::OpenInboxTestListener();
	REQUIRE(listener.has_value());
	ve::network::MultiplayerClient client;
	REQUIRE(client.Connect(ve::tests::InboxTestEndpoint(*listener), "inbox-client"));
	auto peer = listener->Accept();
	REQUIRE(peer.has_value());
	REQUIRE(ve::network::ReceiveNetworkMessage(*peer).has_value());
	REQUIRE(ve::tests::SendInboxOverflowBurst(*peer));
	REQUIRE(ve::tests::WaitForInboxState([&] { return !client.IsConnected(); }));

	const auto messages = client.DrainIncomingMessages();
	CHECK(messages.size() == ve::network::MultiplayerInboxMessageCapacity);
	peer->Close();
	client.Disconnect();
	CHECK(client.DrainIncomingMessages().empty());
}
