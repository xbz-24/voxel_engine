#include "NetworkSessionPumpAcceptance.h"

#include "NetworkBlockReplication.h"

namespace ve::network::detail
{
	std::optional<ve::gameplay::BlockInteraction> TryAcceptBlockMutationForPump(
		NetworkPumpStats& stats,
		const NetworkMessage& message,
		NetworkSequenceTracker& sequenceTracker,
		std::size_t& acceptedMutationCount,
		std::size_t maxAcceptedMutationCount)
	{
		if (message.messageType != NetworkMessageType::BlockMutation)
		{
			stats.messagesIgnored++;
			return std::nullopt;
		}
		const std::optional<ve::gameplay::BlockInteraction> interaction =
			TryReadBlockMutationMessage(message);
		if (!interaction)
		{
			stats.invalidMessagesRejected++;
			return std::nullopt;
		}
		if (acceptedMutationCount >= maxAcceptedMutationCount)
		{
			stats.messagesRejectedByRateLimit++;
			return std::nullopt;
		}
		if (!sequenceTracker.TryAccept(message.sequenceNumber))
		{
			stats.messagesRejectedBySequence++;
			return std::nullopt;
		}
		++acceptedMutationCount;
		return interaction;
	}
}
