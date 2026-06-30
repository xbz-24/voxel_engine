#include "NetworkSequenceTracker.h"

namespace ve::network
{
	bool NetworkSequenceTracker::CanAccept(std::uint32_t sequenceNumber) const noexcept
	{
		return sequenceNumber > lastAcceptedSequenceNumber_;
	}

	bool NetworkSequenceTracker::TryAccept(std::uint32_t sequenceNumber) noexcept
	{
		if (!CanAccept(sequenceNumber)) return false;
		lastAcceptedSequenceNumber_ = sequenceNumber;
		return true;
	}

	void NetworkSequenceTracker::Reset() noexcept
	{
		lastAcceptedSequenceNumber_ = 0;
	}

	std::uint32_t NetworkSequenceTracker::LastAcceptedSequenceNumber() const noexcept
	{
		return lastAcceptedSequenceNumber_;
	}
}
