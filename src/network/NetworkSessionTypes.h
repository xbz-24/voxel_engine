#pragma once

#include "NetworkTypes.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace ve::network
{
	/** High-level networking role for the current runtime. */
	enum class NetworkSessionMode
	{
		Offline,
		Hosting,
		Joined
	};

	/** Authentication policy for accepting or joining sessions. */
	enum class NetworkAuthMode
	{
		NoAuthentication
	};

	/** Policy describing which world state is published over the network. */
	enum class NetworkWorldSnapshotPolicy
	{
		LiveMutationsOnly
	};

	/** Last session-level error reported by host/join operations. */
	enum class NetworkSessionError
	{
		None,
		InvalidHostTickRate,
		HostStartFailed,
		JoinFailed
	};

	/** Lifecycle event kind emitted by NetworkSession. */
	enum class NetworkSessionEventType
	{
		HostingStarted,
		HostStartFailed,
		Joined,
		JoinFailed,
		Stopped
	};

	struct NetworkHostSettings
	{
		std::uint16_t port = 25565;
		int pendingConnectionBacklog = 8;
		std::size_t maxConnectedClients = 8;
		NetworkAuthMode authMode = NetworkAuthMode::NoAuthentication;
		std::uint32_t simulationTickRateHz = 20;
		NetworkWorldSnapshotPolicy worldSnapshotPolicy = NetworkWorldSnapshotPolicy::LiveMutationsOnly;
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

	struct NetworkSessionEvent
	{
		NetworkSessionEventType eventType = NetworkSessionEventType::Stopped;
		NetworkSessionMode mode = NetworkSessionMode::Offline;
		NetworkSessionError error = NetworkSessionError::None;
	};
}
