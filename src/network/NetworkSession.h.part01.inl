
#include "MultiplayerClient.h"
#include "MultiplayerServer.h"
#include "NetworkSequenceTracker.h"
#include "WorldEvent.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace ve::world
{
	class World;
}

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

	/** Settings used when starting a local multiplayer host. */
	struct NetworkHostSettings
	{
		std::uint16_t port = 25565;
		int pendingConnectionBacklog = 8;
		std::size_t maxConnectedClients = 8;
		NetworkAuthMode authMode = NetworkAuthMode::NoAuthentication;
		std::uint32_t simulationTickRateHz = 20;
		NetworkWorldSnapshotPolicy worldSnapshotPolicy = NetworkWorldSnapshotPolicy::LiveMutationsOnly;
	};

	/** Settings used when joining a remote host. */
	struct NetworkJoinSettings
	{
		NetworkEndpoint serverEndpoint;
		std::string playerName;
	};

	/** Counters produced by one network pump pass. */
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
