#pragma once

#include "EntityRegistry.h"

namespace ve::ecs
{
	// TODO: Add component storage and queries so public SceneGraph entities can map onto real runtime entities.
	class EcsWorld
	{
	public:
		/** @return Entity handle created by the registry. */
		[[nodiscard]] Entity CreateEntity();

		/** @param entity Entity to destroy. @return True when destroyed. */
		bool DestroyEntity(Entity entity);

		/** @param entity Entity to inspect. @return True when alive. */
		[[nodiscard]] bool IsAlive(Entity entity) const noexcept;

		/** @return Number of currently alive entities. */
		[[nodiscard]] ve::core::Index AliveCount() const noexcept;

	private:
		// TODO: Add generation-safe handles to public API before exposing entity ids outside the engine.
		EntityRegistry registry_;
	};
}
