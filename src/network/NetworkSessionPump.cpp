#include "NetworkSession.h"

#include "NetworkBlockReplication.h"
#include "NetworkSessionPumpAcceptance.h"
#include "World.h"

#include <unordered_map>

namespace ve::network
{
	NetworkPumpStats NetworkSession::ApplyIncomingMessages(ve::world::World& world)
	{
		if (_mode == NetworkSessionMode::Hosting) return ApplyServerMessages(world);
		if (_mode == NetworkSessionMode::Joined) return ApplyClientMessages(world);
		return {};
	}

	NetworkPumpStats NetworkSession::ApplyServerMessages(ve::world::World& world)
	{
		constexpr std::size_t maxMutationsPerPeer = 64;
		NetworkPumpStats stats;
		std::unordered_map<std::uint32_t, std::size_t> acceptedCountByConnection;
		for (const MultiplayerInboundMessage& inbound : _server.DrainIncomingMessages())
		{
			stats.messagesReceived++;
			std::size_t& acceptedCount = acceptedCountByConnection[inbound.connectionId];
			NetworkSequenceTracker& sequenceTracker =
				_clientSequenceTrackersByConnectionId[inbound.connectionId];
			const std::optional<ve::gameplay::BlockInteraction> interaction =
				detail::TryAcceptBlockMutationForPump(
					stats,
					inbound.message,
					sequenceTracker,
					acceptedCount,
					maxMutationsPerPeer);
			if (!interaction) continue;
			if (ApplyNetworkBlockMutation(world, *interaction))
			{
				stats.blockMutationsApplied++;
				stats.messagesPublished += _server.BroadcastExcept(inbound.connectionId, inbound.message);
			}
			else
			{
				stats.invalidMessagesRejected++;
			}
		}
		return stats;
	}
}
