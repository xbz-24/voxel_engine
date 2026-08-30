#include "EcsWorld.h"

namespace ve::ecs
{
	/// Creates an entity handle.
	Entity EcsWorld::CreateEntity() { return registry_.Create(); }

	/// Destroys an entity handle and removes its components.
	bool EcsWorld::DestroyEntity(Entity entity)
	{
		if (!registry_.Destroy(entity)) return false;
		componentPools_.RemoveEntity(entity);
		return true;
	}

	/// Reports whether an entity is alive.
	bool EcsWorld::IsAlive(Entity entity) const noexcept { return registry_.IsAlive(entity); }

	/// Returns the number of currently alive entities.
	ve::core::Index EcsWorld::AliveCount() const noexcept { return registry_.AliveCount(); }
}
