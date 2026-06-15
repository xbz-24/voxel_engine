#pragma once

#include "CoreTypes.h"
#include "Entity.h"

namespace ve::ecs
{
	class EntityRegistry
	{
	public:
		/** @return New or recycled entity handle. */
		[[nodiscard]] Entity Create();

		/** @param entity Entity handle to destroy. @return True when it was alive. */
		bool Destroy(Entity entity);

		/** @param entity Entity handle to test. @return True when still alive. */
		[[nodiscard]] bool IsAlive(Entity entity) const noexcept;

		/** @return Number of currently alive entities. */
		[[nodiscard]] ve::core::Index AliveCount() const noexcept;

	private:
		struct EntitySlot
		{
			std::uint32_t generation = 1;
			bool is_alive = false;
		};

		ve::core::DynamicArray<EntitySlot> slots_;
		ve::core::DynamicArray<std::uint32_t> free_ids_;
		ve::core::Index alive_count_ = 0;
	};
}
