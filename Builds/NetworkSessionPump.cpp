#include "NetworkSession.h"

#include "NetworkBlockReplication.h"
#include "World.h"

namespace ve::network
{
	NetworkPumpStats NetworkSession::ApplyIncomingMessages(ve::world::World& world)
	{
		if (_mode == NetworkSessionMode::Hosting)
		{
			return ApplyServerMessages(world);
		}
		if (_mode == NetworkSessionMode::Joined)
		{
			return ApplyClientMessages(world);
		}
		return NetworkPumpStats{};
	}

	NetworkPumpStats NetworkSession::ApplyServerMessages(ve::world::World& world)
	{
		NetworkPumpStats stats;
		for (const MultiplayerInboundMessage& inboundMessage : _server.DrainIncomingMessages())
		{
			stats.messagesReceived++;
			if (ApplyNetworkBlockMutation(world, inboundMessage.message))
			{
				stats.blockMutationsApplied++;
				_server.Broadcast(inboundMessage.message);
				stats.messagesPublished++;
			}
		}
		return stats;
	}

	NetworkPumpStats NetworkSession::ApplyClientMessages(ve::world::World& world)
	{
		NetworkPumpStats stats;
		for (const NetworkMessage& message : _client.DrainIncomingMessages())
		{
			stats.messagesReceived++;
			if (ApplyNetworkBlockMutation(world, message))
			{
				stats.blockMutationsApplied++;
			}
		}
		return stats;
	}
}
