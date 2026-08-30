#include <doctest/doctest.h>

#include "NetworkTcpSocket.h"

#include <array>
#include <chrono>
#include <cstddef>
#include <future>
#include <optional>
#include <stop_token>
#include <thread>

namespace
{
	std::optional<ve::network::TcpSocket> OpenListener()
	{
		return ve::network::TcpSocket::Listen({ { "127.0.0.1", 0 }, 1 });
	}

	ve::network::NetworkEndpoint LoopbackEndpoint(const ve::network::TcpSocket& listener)
	{
		const auto endpoint = listener.LocalEndpoint();
		REQUIRE(endpoint.has_value());
		return { "127.0.0.1", endpoint->port };
	}
}

TEST_CASE("tcp stream shutdown interrupts a synchronous receive before close")
{
	auto listener = OpenListener();
	REQUIRE(listener.has_value());
	auto client = ve::network::TcpSocket::Connect(LoopbackEndpoint(*listener));
	REQUIRE(client.has_value());
	auto peer = listener->Accept();
	REQUIRE(peer.has_value());
	std::array<std::byte, 1> destination{};
	std::promise<void> receive_started;
	std::future<bool> receive_result = std::async(std::launch::async, [&]
	{
		receive_started.set_value();
		return peer->ReceiveBytes(destination);
	});
	receive_started.get_future().wait();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	CHECK(receive_result.wait_for(std::chrono::milliseconds(20)) ==
		std::future_status::timeout);

	peer->Shutdown();
	if (receive_result.wait_for(std::chrono::seconds(2)) != std::future_status::ready)
	{
		client->Close();
	}
	REQUIRE(receive_result.wait_for(std::chrono::seconds(2)) ==
		std::future_status::ready);
	CHECK_FALSE(receive_result.get());
	CHECK(peer->IsOpen());
	peer->Close();
	CHECK_FALSE(peer->IsOpen());
}

TEST_CASE("stop-aware accept returns without closing its listener concurrently")
{
	auto listener = OpenListener();
	REQUIRE(listener.has_value());
	const auto endpoint = LoopbackEndpoint(*listener);
	std::stop_source stop_source;
	std::promise<void> accept_started;
	std::future<std::optional<ve::network::TcpSocket>> accept_result =
		std::async(std::launch::async, [&]
		{
			accept_started.set_value();
			return listener->Accept(stop_source.get_token());
		});
	accept_started.get_future().wait();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	CHECK(accept_result.wait_for(std::chrono::milliseconds(20)) ==
		std::future_status::timeout);
	stop_source.request_stop();
	std::optional<ve::network::TcpSocket> rescue_client;
	if (accept_result.wait_for(std::chrono::seconds(2)) != std::future_status::ready)
	{
		rescue_client = ve::network::TcpSocket::Connect(endpoint);
	}
	REQUIRE(accept_result.wait_for(std::chrono::seconds(2)) ==
		std::future_status::ready);
	CHECK_FALSE(accept_result.get().has_value());
	CHECK(listener->IsOpen());
	listener->Close();
	CHECK_FALSE(listener->IsOpen());
}
