#pragma once

#include "MultiplayerClient.h"
#include "MultiplayerServer.h"
#include "WorldEvent.h"

#include <cstddef>
#include <span>
#include <string>

namespace ve::world
{
	class World;
}

namespace ve::network
{
	enum class NetworkSessionMode
	{
		Offline,
		Hosting,
		Joined
	};

	struct NetworkHostSettings
	{
		std::uint16_t port = 25565;
		int pendingConnectionBacklog = 8;
	};

	struct NetworkJoinSettings
	{
		NetworkEndpoint serverEndpoint;
		std::string playerName;
	};

	struct NetworkPumpStats
	{
		std::size_t messagesReceived = 0;
		std::size_t blockMutationsApplied = 0;
		std::size_t messagesPublished = 0;
	};

	class NetworkSession
	{
	public:
		/** @param settings Listen port and pending connection backlog. @return True when hosting started. */
		bool HostGame(const NetworkHostSettings& settings);
		/** @param settings Server endpoint plus local player name. @return True when the client connected. */
		bool JoinGame(const NetworkJoinSettings& settings);
		/** Stops client/server workers and returns to offline mode. */
		void Stop();
		/** @param worldEvents Events drained from the world. @return Number of messages published. */
		std::size_t PublishWorldEvents(std::span<const ve::world::WorldEvent> worldEvents);
		/** @param world World receiving remote mutations. @return Receive/apply counters for this pump. */
		NetworkPumpStats ApplyIncomingMessages(ve::world::World& world);
		/** @return Current high-level network mode. */
		NetworkSessionMode Mode() const noexcept;
		/** @return True when hosting or connected as a client. */
		bool IsOnline() const noexcept;

	private:
		std::size_t PublishMessage(const NetworkMessage& message);
		NetworkPumpStats ApplyServerMessages(ve::world::World& world);
		NetworkPumpStats ApplyClientMessages(ve::world::World& world);

		NetworkSessionMode _mode = NetworkSessionMode::Offline;
		MultiplayerClient _client;
		MultiplayerServer _server;
	};
}
