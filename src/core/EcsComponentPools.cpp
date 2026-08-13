#include "EcsComponentPools.h"

namespace ve::ecs::detail
{
	void ComponentPools::RemoveEntity(Entity entity) noexcept
	{
		for (auto& [componentType, componentPool] : pools_)
		{
			(void)componentType;
			componentPool->RemoveEntity(entity);
		}
	}
}
