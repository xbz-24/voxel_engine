#pragma once

#include "CoreTypes.h"
#include "LinearMemoryResource.h"

#include <cstddef>
#include <vector>

namespace ve::memory
{
	class MemoryArena
	{
	public:
		/** @param byte_capacity Number of bytes reserved by the arena. */
		explicit MemoryArena(ve::core::ByteCount byte_capacity);

		/** @return Reset-only memory resource backed by this arena. */
		[[nodiscard]] LinearMemoryResource& Resource() noexcept;

		/** @return Bytes reserved by the arena. */
		[[nodiscard]] ve::core::ByteCount Capacity() const noexcept;

		/** @return Bytes currently used by linear allocations. */
		[[nodiscard]] ve::core::ByteCount BytesUsed() const noexcept;

		/** Releases all allocations made from this arena. */
		void Reset() noexcept;

	private:
		std::vector<std::byte> backing_store_;
		LinearMemoryResource resource_;
	};
}
