#include "NetworkSession.h"

#include "NetworkBlockReplication.h"
#include "World.h"

#include <cstddef>
#include <cstdint>
#include <optional>
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

		std::optional<ve::gameplay::BlockInteraction> TryAcceptBlockMutationForPump(
			NetworkPumpStats& pumpStats,
			const NetworkMessage& message,
			NetworkSequenceTracker& sequenceTracker,
			std::size_t& acceptedBlockMutationCount,
			std::size_t maxAcceptedBlockMutationCount)
		{
			if (!IsBlockMutationMessage(message))
			{
				pumpStats.messagesIgnored++;
				return std::nullopt;
			}
			std::optional<ve::gameplay::BlockInteraction> blockInteraction = TryReadBlockMutationMessage(message);
			if (!blockInteraction)
			{
				pumpStats.invalidMessagesRejected++;
				return std::nullopt;
			}
			if (acceptedBlockMutationCount >= maxAcceptedBlockMutationCount)
			{
				pumpStats.messagesRejectedByRateLimit++;
				return std::nullopt;
			}
			if (!sequenceTracker.TryAccept(message.sequenceNumber))
			{
				pumpStats.messagesRejectedBySequence++;
				return std::nullopt;
			}
			++acceptedBlockMutationCount;
			return blockInteraction;
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
		std::unordered_map<std::uint32_t, std::size_t> acceptedBlockMutationCountByConnectionId;
		for (const MultiplayerInboundMessage& inboundMessage : _server.DrainIncomingMessages())
		{
			pumpStats.messagesReceived++;
			std::size_t& acceptedBlockMutationCount =
				acceptedBlockMutationCountByConnectionId[inboundMessage.connectionId];
			NetworkSequenceTracker& clientSequenceTracker =
				_clientSequenceTrackersByConnectionId[inboundMessage.connectionId];
			const std::optional<ve::gameplay::BlockInteraction> blockInteraction = TryAcceptBlockMutationForPump(
				pumpStats,
				inboundMessage.message,
				clientSequenceTracker,
				acceptedBlockMutationCount,
				MaxInboundBlockMutationsAppliedPerPeerPerPump);
			if (!blockInteraction) continue;
			if (ApplyNetworkBlockMutation(world, *blockInteraction))
			{
				pumpStats.blockMutationsApplied++;
				pumpStats.messagesPublished += _server.BroadcastExcept(inboundMessage.connectionId, inboundMessage.message);
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
		std::size_t acceptedBlockMutationCountForClient = 0;
		for (const NetworkMessage& message : _client.DrainIncomingMessages())
		{
			pumpStats.messagesReceived++;
			const std::optional<ve::gameplay::BlockInteraction> blockInteraction = TryAcceptBlockMutationForPump(
				pumpStats,
				message,
				_serverToClientSequenceTracker,
				acceptedBlockMutationCountForClient,
				MaxInboundBlockMutationsAppliedToClientPerPump);
			if (!blockInteraction) continue;
			if (ApplyNetworkBlockMutation(world, *blockInteraction))
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
