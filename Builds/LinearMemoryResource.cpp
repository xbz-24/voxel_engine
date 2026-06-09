#include "LinearMemoryResource.h"

#include <new>

namespace
{
	/// Rounds an address up to the requested alignment.
	std::size_t AlignForward(std::size_t address, std::size_t alignment) noexcept
	{
		const std::size_t mask = alignment - 1;
		return (address + mask) & ~mask;
	}
}

namespace ve::memory
{
	/// Creates a linear allocator over caller-owned memory.
	LinearMemoryResource::LinearMemoryResource(std::span<std::byte> backing_store) noexcept
		: backing_store_(backing_store)
	{
	}

	/// Releases all linear allocations at once.
	void LinearMemoryResource::Reset() noexcept { next_byte_ = 0; }

	/// Returns bytes already handed out by this allocator.
	ve::core::ByteCount LinearMemoryResource::BytesUsed() const noexcept { return next_byte_; }

	/// Returns total bytes available in the backing store.
	ve::core::ByteCount LinearMemoryResource::Capacity() const noexcept { return backing_store_.size(); }

	/// Allocates aligned memory from the current linear cursor.
	void* LinearMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment)
	{
		const std::size_t aligned_offset = AlignForward(next_byte_, alignment);
		const std::size_t next_offset = aligned_offset + bytes;
		if (next_offset > backing_store_.size()) throw std::bad_alloc{};
		next_byte_ = next_offset;
		return backing_store_.data() + aligned_offset;
	}

	/// Ignores individual frees because this is a reset-only allocator.
	void LinearMemoryResource::do_deallocate(void*, std::size_t, std::size_t) {}

	/// Reports whether another resource is this same allocator object.
	bool LinearMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept
	{
		return this == &other;
	}
}
