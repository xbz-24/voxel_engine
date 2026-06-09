#pragma once

#include "CoreTypes.h"

#include <cstddef>
#include <memory_resource>
#include <span>

namespace ve::memory
{
	class LinearMemoryResource final : public std::pmr::memory_resource
	{
	public:
		/** @param backing_store Contiguous memory owned by the caller. */
		explicit LinearMemoryResource(std::span<std::byte> backing_store) noexcept;

		/** Releases all linear allocations at once. */
		void Reset() noexcept;

		/** @return Bytes already handed out by this allocator. */
		[[nodiscard]] ve::core::ByteCount BytesUsed() const noexcept;

		/** @return Total bytes available in the backing store. */
		[[nodiscard]] ve::core::ByteCount Capacity() const noexcept;

	private:
		/** @param bytes Allocation size. @param alignment Required alignment. @return Aligned memory. */
		void* do_allocate(std::size_t bytes, std::size_t alignment) override;

		/** @param pointer Ignored pointer. @param bytes Ignored size. @param alignment Ignored alignment. */
		void do_deallocate(void* pointer, std::size_t bytes, std::size_t alignment) override;

		/** @param other Compared memory resource. @return True when comparing this object. */
		[[nodiscard]] bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

		std::span<std::byte> backing_store_;
		ve::core::ByteCount next_byte_ = 0;
	};
}
