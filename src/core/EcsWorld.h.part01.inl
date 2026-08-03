
#include "EntityRegistry.h"

#include <memory>
#include <optional>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace ve::ecs
{
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

		/** @param entity Owner entity. @param args Component constructor arguments. @return Stored component, or null for a dead handle. */
		template <typename Component, typename... Args>
		[[nodiscard]] Component* EmplaceComponent(Entity entity, Args&&... args)
		{
			if (!IsAlive(entity)) return nullptr;
			return &ComponentPoolFor<Component>().Emplace(entity, std::forward<Args>(args)...);
		}

		/** @param entity Owner entity. @return True when a component was removed. */
		template <typename Component>
		bool RemoveComponent(Entity entity)
		{
			if (ComponentPool<Component>* pool = FindComponentPool<Component>()) return pool->Remove(entity);
			return false;
		}

		/** @param entity Owner entity. @return Stored component, or null when absent/stale. */
		template <typename Component>
		[[nodiscard]] Component* FindComponent(Entity entity) noexcept
		{
			ComponentPool<Component>* pool = FindComponentPool<Component>();
			return pool != nullptr ? pool->Find(entity) : nullptr;
		}

		/** @param entity Owner entity. @return Stored component, or null when absent/stale. */
		template <typename Component>
		[[nodiscard]] const Component* FindComponent(Entity entity) const noexcept
		{
			const ComponentPool<Component>* pool = FindComponentPool<Component>();
			return pool != nullptr ? pool->Find(entity) : nullptr;
		}

		/** @param entity Owner entity. @return True when the entity has this component. */
		template <typename Component>
		[[nodiscard]] bool HasComponent(Entity entity) const noexcept
		{
			return FindComponent<Component>(entity) != nullptr;
		}

		/** @return Number of live components of this type. */
		template <typename Component>
		[[nodiscard]] ve::core::Index ComponentCount() const noexcept
		{
			const ComponentPool<Component>* pool = FindComponentPool<Component>();
			return pool != nullptr ? pool->Count() : 0;
		}

	private:
		struct IComponentPool
		{
			virtual ~IComponentPool() = default;
			virtual void RemoveEntity(Entity entity) noexcept = 0;
		};

		template <typename Component>
		class ComponentPool final : public IComponentPool
		{
		public:
			template <typename... Args>
			Component& Emplace(Entity entity, Args&&... args)
			{
