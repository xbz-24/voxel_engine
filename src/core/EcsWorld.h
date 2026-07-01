#pragma once

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
				EnsureSlot(entity.id);
				ComponentSlot& slot = slots_[entity.id];
				if (!slot.component.has_value()) ++live_component_count_;
				slot.generation = entity.generation;
				slot.component.emplace(std::forward<Args>(args)...);
				return *slot.component;
			}

			bool Remove(Entity entity) noexcept
			{
				if (Component* component = Find(entity))
				{
					(void)component;
					slots_[entity.id].component.reset();
					--live_component_count_;
					return true;
				}
				return false;
			}

			void RemoveEntity(Entity entity) noexcept override
			{
				Remove(entity);
			}

			[[nodiscard]] Component* Find(Entity entity) noexcept
			{
				if (entity.id >= slots_.size()) return nullptr;
				ComponentSlot& slot = slots_[entity.id];
				if (slot.generation != entity.generation || !slot.component.has_value()) return nullptr;
				return &*slot.component;
			}

			[[nodiscard]] const Component* Find(Entity entity) const noexcept
			{
				if (entity.id >= slots_.size()) return nullptr;
				const ComponentSlot& slot = slots_[entity.id];
				if (slot.generation != entity.generation || !slot.component.has_value()) return nullptr;
				return &*slot.component;
			}

			[[nodiscard]] ve::core::Index Count() const noexcept
			{
				return live_component_count_;
			}

		private:
			struct ComponentSlot
			{
				std::uint32_t generation = 0;
				std::optional<Component> component;
			};

			void EnsureSlot(std::uint32_t entity_id)
			{
				if (entity_id >= slots_.size()) slots_.resize(static_cast<ve::core::Index>(entity_id) + 1U);
			}

			ve::core::DynamicArray<ComponentSlot> slots_;
			ve::core::Index live_component_count_ = 0;
		};

		template <typename Component>
		[[nodiscard]] ComponentPool<Component>& ComponentPoolFor()
		{
			const std::type_index component_type(typeid(Component));
			auto existing_pool = component_pools_.find(component_type);
			if (existing_pool != component_pools_.end())
			{
				return static_cast<ComponentPool<Component>&>(*existing_pool->second);
			}
			auto [new_pool, inserted] = component_pools_.emplace(
				component_type,
				std::make_unique<ComponentPool<Component>>());
			(void)inserted;
			return static_cast<ComponentPool<Component>&>(*new_pool->second);
		}

		template <typename Component>
		[[nodiscard]] ComponentPool<Component>* FindComponentPool() noexcept
		{
			const auto existing_pool = component_pools_.find(std::type_index(typeid(Component)));
			if (existing_pool == component_pools_.end()) return nullptr;
			return static_cast<ComponentPool<Component>*>(existing_pool->second.get());
		}

		template <typename Component>
		[[nodiscard]] const ComponentPool<Component>* FindComponentPool() const noexcept
		{
			const auto existing_pool = component_pools_.find(std::type_index(typeid(Component)));
			if (existing_pool == component_pools_.end()) return nullptr;
			return static_cast<const ComponentPool<Component>*>(existing_pool->second.get());
		}

		EntityRegistry registry_;
		std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> component_pools_;
	};
}
