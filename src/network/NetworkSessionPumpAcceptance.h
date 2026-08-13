#pragma once

#include "BlockInteraction.h"
#include "NetworkSequenceTracker.h"
#include "NetworkSessionTypes.h"

#include <optional>

namespace ve::network::detail
{
	std::optional<ve::gameplay::BlockInteraction> TryAcceptBlockMutationForPump(
		NetworkPumpStats& pumpStats,
		const NetworkMessage& message,
		NetworkSequenceTracker& sequenceTracker,
		std::size_t& acceptedBlockMutationCount,
		std::size_t maxAcceptedBlockMutationCount);
}
