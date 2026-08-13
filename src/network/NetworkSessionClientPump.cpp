#include "NetworkSession.h"

#include "NetworkBlockReplication.h"
#include "NetworkSessionPumpAcceptance.h"
#include "World.h"

namespace ve::network
{
	NetworkPumpStats NetworkSession::ApplyClientMessages(ve::world::World& world)
	{
		constexpr std::size_t maxMutationsPerPump = 128;
		NetworkPumpStats stats;
		std::size_t acceptedMutationCount = 0;
		for (const NetworkMessage& message : _client.DrainIncomingMessages())
		{
			stats.messagesReceived++;
			const std::optional<ve::gameplay::BlockInteraction> interaction =
				detail::TryAcceptBlockMutationForPump(
					stats,
					message,
					_serverToClientSequenceTracker,
					acceptedMutationCount,
					maxMutationsPerPump);
			if (!interaction) continue;
			if (ApplyNetworkBlockMutation(world, *interaction))
			{
				stats.blockMutationsApplied++;
			}
			else
			{
				stats.invalidMessagesRejected++;
			}
		}
		return stats;
	}
}
