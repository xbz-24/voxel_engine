#include "MemoryArena.h"

namespace ve::memory
{
	/// Creates an owned linear allocation arena.
	MemoryArena::MemoryArena(ve::core::ByteCount byte_capacity)
		: backing_store_(byte_capacity),
		  resource_(std::span<std::byte>(backing_store_.data(), backing_store_.size()))
	{
	}

	/// Returns the reset-only memory resource backed by this arena.
	LinearMemoryResource& MemoryArena::Resource() noexcept { return resource_; }

	/// Returns bytes reserved by the arena.
	ve::core::ByteCount MemoryArena::Capacity() const noexcept { return resource_.Capacity(); }

	/// Returns bytes currently used by linear allocations.
	ve::core::ByteCount MemoryArena::BytesUsed() const noexcept { return resource_.BytesUsed(); }

	/// Releases all allocations made from this arena.
	void MemoryArena::Reset() noexcept { resource_.Reset(); }
}
