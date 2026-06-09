#include "EcsWorld.h"

namespace ve::ecs
{
	/// Creates an entity handle.
	Entity EcsWorld::CreateEntity() { return registry_.Create(); }

	/// Destroys an entity handle.
	bool EcsWorld::DestroyEntity(Entity entity) { return registry_.Destroy(entity); }

	/// Reports whether an entity is alive.
	bool EcsWorld::IsAlive(Entity entity) const noexcept { return registry_.IsAlive(entity); }

	/// Returns the number of currently alive entities.
	ve::core::Index EcsWorld::AliveCount() const noexcept { return registry_.AliveCount(); }
}
