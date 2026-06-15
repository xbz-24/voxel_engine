#pragma once

#include "CoreTypes.h"

#include <cstdint>
#include <optional>

namespace ve::rendering
{
	struct GraphicsResourceHandle
	{
		std::uint32_t index = UINT32_MAX;
		std::uint32_t generation = 0;

		/** @return True when the handle points at a possible slot. */
		[[nodiscard]] bool IsValid() const noexcept { return index != UINT32_MAX; }

		/** @param other Handle to compare. @return True when both refer to the same generation. */
		[[nodiscard]] bool operator==(const GraphicsResourceHandle& other) const noexcept = default;
	};

	template <typename Resource>
	class GraphicsResourcePool
	{
	public:
		/** @param resource Resource value to store. @return Stable generational handle. */
		[[nodiscard]] GraphicsResourceHandle Create(Resource resource)
		{
			const std::uint32_t slot_index = AcquireSlot();
			Slot& slot = slots_[slot_index];
			slot.resource.emplace(ve::core::Move(resource));
			slot.is_alive = true;
			live_count_++;
			return { slot_index, slot.generation };
		}

		/** @param handle Resource handle. @return Resource pointer, or null when stale. */
		[[nodiscard]] Resource* TryGet(GraphicsResourceHandle handle) noexcept
		{
			return IsAlive(handle) ? &slots_[handle.index].resource.value() : nullptr;
		}

		/** @param handle Resource handle. @return Read-only resource pointer, or null when stale. */
		[[nodiscard]] const Resource* TryGet(GraphicsResourceHandle handle) const noexcept
		{
			return IsAlive(handle) ? &slots_[handle.index].resource.value() : nullptr;
		}

		/** @param handle Resource handle. @return True when a live resource was removed. */
		bool Destroy(GraphicsResourceHandle handle)
		{
			if (!IsAlive(handle)) return false;
			Slot& slot = slots_[handle.index];
			slot.is_alive = false;
			slot.resource.reset();
			slot.generation++;
			free_slots_.push_back(handle.index);
			live_count_--;
			return true;
		}

		/** @return Number of live resources. */
		[[nodiscard]] ve::core::Index Count() const noexcept { return live_count_; }

	private:
		struct Slot
		{
			std::optional<Resource> resource;
			std::uint32_t generation = 1;
			bool is_alive = false;
		};

		/** @param handle Resource handle to validate. @return True when handle points at live slot. */
		[[nodiscard]] bool IsAlive(GraphicsResourceHandle handle) const noexcept
		{
			return handle.index < slots_.size() && slots_[handle.index].is_alive &&
				slots_[handle.index].generation == handle.generation &&
				slots_[handle.index].resource.has_value();
		}

		/** @return Free slot index reused from the list or appended at the end. */
		[[nodiscard]] std::uint32_t AcquireSlot()
		{
			if (free_slots_.empty())
			{
				slots_.push_back(Slot{});
				return static_cast<std::uint32_t>(slots_.size() - 1U);
			}
			const std::uint32_t slot_index = free_slots_.back();
			free_slots_.pop_back();
			return slot_index;
		}

		ve::core::DynamicArray<Slot> slots_;
		ve::core::DynamicArray<std::uint32_t> free_slots_;
		ve::core::Index live_count_ = 0;
	};
}
