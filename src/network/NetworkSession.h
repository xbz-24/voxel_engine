#pragma once

#include "MultiplayerClient.h"
#include "MultiplayerServer.h"
#include "NetworkSequenceTracker.h"
#include "WorldEvent.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <unordered_map>

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
		// TODO: Add auth mode, tick rate, and world snapshot policy.
		std::uint16_t port = 25565;
		int pendingConnectionBacklog = 8;
		std::size_t maxConnectedClients = 8;
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
		std::size_t messagesIgnored = 0;
		std::size_t messagesRejectedByRateLimit = 0;
		std::size_t messagesRejectedBySequence = 0;
		std::size_t invalidMessagesRejected = 0;
	};

	class NetworkSession
	{
	public:
		// TODO: Expose connection state and errors through public API events instead of only internal stats.
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
		// TODO: Queue outbound world snapshots separately from live mutation packets.
		std::size_t PublishMessage(const NetworkMessage& message);
		NetworkPumpStats ApplyServerMessages(ve::world::World& world);
		NetworkPumpStats ApplyClientMessages(ve::world::World& world);

		NetworkSessionMode _mode = NetworkSessionMode::Offline;
		MultiplayerClient _client;
		MultiplayerServer _server;
		NetworkSequenceTracker _serverToClientSequenceTracker;
		std::unordered_map<std::uint32_t, NetworkSequenceTracker> _clientSequenceTrackersByConnectionId;
	};
}
