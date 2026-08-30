#pragma once

#include "EcsComponentPool.h"

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace ve::ecs::detail
{
	class ComponentPools
	{
	public:
		template <typename Component, typename... Args>
		Component& Emplace(Entity entity, Args&&... args)
		{
			return PoolFor<Component>().Emplace(entity, std::forward<Args>(args)...);
		}

		template <typename Component>
		bool Remove(Entity entity)
		{
			ComponentPool<Component>* pool = FindPool<Component>();
			return pool != nullptr && pool->Remove(entity);
		}

		template <typename Component>
		[[nodiscard]] Component* Find(Entity entity) noexcept
		{
			ComponentPool<Component>* pool = FindPool<Component>();
			return pool != nullptr ? pool->Find(entity) : nullptr;
		}

		template <typename Component>
		[[nodiscard]] const Component* Find(Entity entity) const noexcept
		{
			const ComponentPool<Component>* pool = FindPool<Component>();
			return pool != nullptr ? pool->Find(entity) : nullptr;
		}

		template <typename Component>
		[[nodiscard]] ve::core::Index Count() const noexcept
		{
			const ComponentPool<Component>* pool = FindPool<Component>();
			return pool != nullptr ? pool->Count() : 0;
		}

		void RemoveEntity(Entity entity) noexcept;

	private:
		template <typename Component>
		ComponentPool<Component>& PoolFor()
		{
			const std::type_index type(typeid(Component));
			auto existing = pools_.find(type);
			if (existing != pools_.end()) return static_cast<ComponentPool<Component>&>(*existing->second);
			auto [created, inserted] = pools_.emplace(type, std::make_unique<ComponentPool<Component>>());
			(void)inserted;
			return static_cast<ComponentPool<Component>&>(*created->second);
		}

		template <typename Component>
		ComponentPool<Component>* FindPool() noexcept
		{
			const auto existing = pools_.find(std::type_index(typeid(Component)));
			return existing == pools_.end()
				? nullptr
				: static_cast<ComponentPool<Component>*>(existing->second.get());
		}

		template <typename Component>
		const ComponentPool<Component>* FindPool() const noexcept
		{
			const auto existing = pools_.find(std::type_index(typeid(Component)));
			return existing == pools_.end()
				? nullptr
				: static_cast<const ComponentPool<Component>*>(existing->second.get());
		}

		std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools_;
	};
}
