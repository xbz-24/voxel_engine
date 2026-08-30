#pragma once

#include "EcsComponentPools.h"
#include "EntityRegistry.h"

#include <utility>

namespace ve::ecs
{
	class EcsWorld
	{
	public:
		[[nodiscard]] Entity CreateEntity();
		bool DestroyEntity(Entity entity);
		[[nodiscard]] bool IsAlive(Entity entity) const noexcept;
		[[nodiscard]] ve::core::Index AliveCount() const noexcept;

		template <typename Component, typename... Args>
		[[nodiscard]] Component* EmplaceComponent(Entity entity, Args&&... args)
		{
			if (!IsAlive(entity)) return nullptr;
			return &componentPools_.Emplace<Component>(entity, std::forward<Args>(args)...);
		}

		template <typename Component>
		bool RemoveComponent(Entity entity)
		{
			return componentPools_.Remove<Component>(entity);
		}

		template <typename Component>
		[[nodiscard]] Component* FindComponent(Entity entity) noexcept
		{
			return componentPools_.Find<Component>(entity);
		}

		template <typename Component>
		[[nodiscard]] const Component* FindComponent(Entity entity) const noexcept
		{
			return componentPools_.Find<Component>(entity);
		}

		template <typename Component>
		[[nodiscard]] bool HasComponent(Entity entity) const noexcept
		{
			return FindComponent<Component>(entity) != nullptr;
		}

		template <typename Component>
		[[nodiscard]] ve::core::Index ComponentCount() const noexcept
		{
			return componentPools_.Count<Component>();
		}

	private:
		EntityRegistry registry_;
		detail::ComponentPools componentPools_;
	};
}
