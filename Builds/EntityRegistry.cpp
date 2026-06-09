#include "EntityRegistry.h"

namespace ve::ecs
{
	/// Creates a new or recycled entity handle.
	Entity EntityRegistry::Create()
	{
		if (!free_ids_.empty())
		{
			const std::uint32_t id = free_ids_.back();
			free_ids_.pop_back();
			slots_[id].is_alive = true;
			alive_count_++;
			return Entity{ id, slots_[id].generation };
		}
		const std::uint32_t id = static_cast<std::uint32_t>(slots_.size());
		slots_.push_back(EntitySlot{ 1, true });
		alive_count_++;
		return Entity{ id, 1 };
	}

	/// Destroys an alive entity and invalidates old handles.
	bool EntityRegistry::Destroy(Entity entity)
	{
		if (!IsAlive(entity)) return false;
		EntitySlot& slot = slots_[entity.id];
		slot.is_alive = false;
		slot.generation++;
		free_ids_.push_back(entity.id);
		alive_count_--;
		return true;
	}

	/// Reports whether an entity handle is alive.
	bool EntityRegistry::IsAlive(Entity entity) const noexcept
	{
		return entity.id < slots_.size() && slots_[entity.id].is_alive &&
			slots_[entity.id].generation == entity.generation;
	}

	/// Returns the number of currently alive entities.
	ve::core::Index EntityRegistry::AliveCount() const noexcept { return alive_count_; }
}
