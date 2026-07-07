#pragma once

#include <cstdint>

namespace ve::network
{
	/**
	 * Tracks the last accepted packet sequence for one ordered network stream.
	 */
	class NetworkSequenceTracker
	{
	public:
		/** @return True when the sequence number can be accepted. */
		[[nodiscard]] bool CanAccept(std::uint32_t sequenceNumber) const noexcept;

		/** @return True when the sequence was accepted and stored as the newest sequence. */
		bool TryAccept(std::uint32_t sequenceNumber) noexcept;

		/** Clears accepted sequence state for a new connection/session. */
		void Reset() noexcept;

		/** @return Last accepted sequence number, or zero before any accepted packet. */
		[[nodiscard]] std::uint32_t LastAcceptedSequenceNumber() const noexcept;

	private:
		std::uint32_t lastAcceptedSequenceNumber_ = 0;
	};
}
