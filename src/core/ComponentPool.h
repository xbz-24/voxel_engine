#pragma once

#include "CoreTypes.h"
#include "Entity.h"

namespace ve::ecs
{
	template <typename Component>
	class ComponentPool
	{
	public:
		/**
		 * Assigns a component value to an entity.
		 *
		 * @tparam Component Component type stored by this pool.
		 * @param entity Entity receiving the component.
		 * @param component Component value to store.
		 * @return Reference to the stored component.
		 */
		Component& Set(Entity entity, Component component)
		{
			EnsureSlot(entity.id);
			Slot& slot = slots_[entity.id];
			if (!slot.is_occupied) component_count_++;
			slot.owner = entity;
			slot.component = ve::core::Move(component);
			slot.is_occupied = true;
			return slot.component;
		}

		/** @param entity Entity to inspect. @return True when a matching component exists. */
		[[nodiscard]] bool Has(Entity entity) const noexcept
		{
			return entity.id < slots_.size() && slots_[entity.id].is_occupied && slots_[entity.id].owner == entity;
		}

		/** @param entity Entity to inspect. @return Component pointer, or null when missing. */
		[[nodiscard]] Component* TryGet(Entity entity) noexcept
		{
			return Has(entity) ? &slots_[entity.id].component : nullptr;
		}

		/** @param entity Entity to remove from. @return True when a component was removed. */
		bool Remove(Entity entity) noexcept
		{
			if (!Has(entity)) return false;
			slots_[entity.id].is_occupied = false;
			component_count_--;
			return true;
		}

		/** @return Number of stored components. */
		[[nodiscard]] ve::core::Index Count() const noexcept { return component_count_; }

	private:
		struct Slot
		{
			Entity owner{};
			Component component{};
			bool is_occupied = false;
		};

		/** @param id Entity id that must fit in the sparse array. */
		void EnsureSlot(std::uint32_t id)
		{
			if (id >= slots_.size()) slots_.resize(static_cast<ve::core::Index>(id) + 1);
		}

		ve::core::DynamicArray<Slot> slots_;
		ve::core::Index component_count_ = 0;
	};
}
