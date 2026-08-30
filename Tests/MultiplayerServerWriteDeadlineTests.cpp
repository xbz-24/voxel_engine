#include <doctest/doctest.h>

#include "MultiplayerServer.h"
#include "NetworkPacketIO.h"
#include "NetworkSerialization.h"

#include <atomic>
#include <chrono>
#include <cstddef>
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

	template<typename Predicate>
	bool WaitUntil(Predicate predicate, std::chrono::milliseconds timeout)
	{
		const auto deadline = std::chrono::steady_clock::now() + timeout;
		while (!predicate() && std::chrono::steady_clock::now() < deadline)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return predicate();
	}

	bool WaitForStableCount(
		const std::atomic_size_t& count, const std::atomic_bool& running)
	{
		std::size_t previous = count.load();
		int stable_samples = 0;
		const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
		while (running && std::chrono::steady_clock::now() < deadline)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			const std::size_t current = count.load();
			stable_samples = current == previous ? stable_samples + 1 : 0;
			if (running && stable_samples == 3) return true;
			previous = current;
		}
		return false;
	}
}

TEST_CASE("multiplayer server bounds a write to a peer that does not read")
{
	const auto endpoint = ReserveEndpoint();
	ve::network::MultiplayerServer server;
	REQUIRE(server.Start({ endpoint, 1 }, 1));
	auto client = ve::network::TcpSocket::Connect(endpoint);
	REQUIRE(client.has_value());
	const ve::network::NetworkMessage hello{
		ve::network::NetworkMessageType::ClientHello,
		ve::network::SerializeClientHello("deadline-test"), 1 };
	REQUIRE(ve::network::SendNetworkMessage(*client, hello));
	REQUIRE(WaitUntil([&] { return server.Stats().activeClientCount == 1U; },
		std::chrono::seconds(2)));
	const ve::network::NetworkMessage large_message{
		ve::network::NetworkMessageType::Ping,
		ve::network::ByteBuffer(ve::network::MaxPacketPayloadByteCount, std::byte{ 0x2A }) };
	std::atomic_size_t completed_writes = 0;
	std::atomic_bool writing = true;
	std::thread writer([&]
	{
		while (server.Broadcast(large_message) == 1U) ++completed_writes;
		writing = false;
	});
	const bool wrote = WaitUntil([&] { return completed_writes.load() > 0U; },
		std::chrono::seconds(2));
	const bool became_backpressured = wrote && WaitForStableCount(completed_writes, writing);
	const bool timed_out_without_stop = became_backpressured &&
		WaitUntil([&] { return !writing.load(); }, std::chrono::seconds(2));
	if (!timed_out_without_stop) client->Close();
	const bool rescued = WaitUntil([&] { return !writing.load(); }, std::chrono::seconds(2));
	writer.join();

	CHECK(wrote);
	CHECK(became_backpressured);
	CHECK(timed_out_without_stop);
	CHECK(rescued);
	server.Stop();
	client->Close();
}
