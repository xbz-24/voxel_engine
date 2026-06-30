#include "NetworkSession.h"

#include "NetworkBlockReplication.h"
#include "World.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>

namespace ve::network
{
	namespace
	{
		constexpr std::size_t MaxInboundBlockMutationsAppliedPerPeerPerPump = 64;
		constexpr std::size_t MaxInboundBlockMutationsAppliedToClientPerPump = 128;

		bool IsBlockMutationMessage(const NetworkMessage& message) noexcept
		{
			return message.messageType == NetworkMessageType::BlockMutation;
		}

		bool HasValidBlockMutationPayload(const NetworkMessage& message)
		{
			return TryReadBlockMutationMessage(message).has_value();
		}

		bool TryAcceptBlockMutationForPump(
			NetworkPumpStats& pumpStats,
			const NetworkMessage& message,
			NetworkSequenceTracker& sequenceTracker,
			std::size_t& appliedBlockMutationCount,
			std::size_t maxAppliedBlockMutationCount)
		{
			if (!IsBlockMutationMessage(message))
			{
				pumpStats.messagesIgnored++;
				return false;
			}
			if (!HasValidBlockMutationPayload(message))
			{
				pumpStats.invalidMessagesRejected++;
				return false;
			}
			if (appliedBlockMutationCount >= maxAppliedBlockMutationCount)
			{
				pumpStats.messagesRejectedByRateLimit++;
				return false;
			}
			++appliedBlockMutationCount;
			if (!sequenceTracker.TryAccept(message.sequenceNumber))
			{
				pumpStats.messagesRejectedBySequence++;
				return false;
			}
			return true;
		}
	}

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
		NetworkPumpStats pumpStats;
		std::unordered_map<std::uint32_t, std::size_t> appliedBlockMutationCountByConnectionId;
		for (const MultiplayerInboundMessage& inboundMessage : _server.DrainIncomingMessages())
		{
			pumpStats.messagesReceived++;
			std::size_t& appliedBlockMutationCount =
				appliedBlockMutationCountByConnectionId[inboundMessage.connectionId];
			NetworkSequenceTracker& clientSequenceTracker =
				_clientSequenceTrackersByConnectionId[inboundMessage.connectionId];
			if (!TryAcceptBlockMutationForPump(
				pumpStats,
				inboundMessage.message,
				clientSequenceTracker,
				appliedBlockMutationCount,
				MaxInboundBlockMutationsAppliedPerPeerPerPump)) continue;
			if (ApplyNetworkBlockMutation(world, inboundMessage.message))
			{
				pumpStats.blockMutationsApplied++;
				_server.BroadcastExcept(inboundMessage.connectionId, inboundMessage.message);
				pumpStats.messagesPublished++;
			}
			else
			{
				pumpStats.invalidMessagesRejected++;
			}
		}
		return pumpStats;
	}

	NetworkPumpStats NetworkSession::ApplyClientMessages(ve::world::World& world)
	{
		NetworkPumpStats pumpStats;
		std::size_t appliedBlockMutationCountForClient = 0;
		for (const NetworkMessage& message : _client.DrainIncomingMessages())
		{
			pumpStats.messagesReceived++;
			if (!TryAcceptBlockMutationForPump(
				pumpStats,
				message,
				_serverToClientSequenceTracker,
				appliedBlockMutationCountForClient,
				MaxInboundBlockMutationsAppliedToClientPerPump)) continue;
			if (ApplyNetworkBlockMutation(world, message))
			{
				pumpStats.blockMutationsApplied++;
			}
			else
			{
				pumpStats.invalidMessagesRejected++;
			}
		}
		return pumpStats;
	}
}
