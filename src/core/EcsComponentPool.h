#pragma once

#include "CoreTypes.h"
#include "Entity.h"

#include <optional>
#include <utility>

namespace ve::ecs::detail
{
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
			if (!slot.component) ++liveComponentCount_;
			slot.generation = entity.generation;
			slot.component.emplace(std::forward<Args>(args)...);
			return *slot.component;
		}

		bool Remove(Entity entity) noexcept
		{
			if (!Find(entity)) return false;
			slots_[entity.id].component.reset();
			--liveComponentCount_;
			return true;
		}

		void RemoveEntity(Entity entity) noexcept override
		{
			Remove(entity);
		}

		[[nodiscard]] Component* Find(Entity entity) noexcept
		{
			if (entity.id >= slots_.size()) return nullptr;
			ComponentSlot& slot = slots_[entity.id];
			if (slot.generation != entity.generation || !slot.component) return nullptr;
			return &*slot.component;
		}

		[[nodiscard]] const Component* Find(Entity entity) const noexcept
		{
			if (entity.id >= slots_.size()) return nullptr;
			const ComponentSlot& slot = slots_[entity.id];
			if (slot.generation != entity.generation || !slot.component) return nullptr;
			return &*slot.component;
		}

		[[nodiscard]] ve::core::Index Count() const noexcept
		{
			return liveComponentCount_;
		}

	private:
		struct ComponentSlot
		{
			std::uint32_t generation = 0;
			std::optional<Component> component;
		};

		void EnsureSlot(std::uint32_t entityId)
		{
			if (entityId >= slots_.size()) slots_.resize(ve::core::ToIndex(entityId) + 1U);
		}

		ve::core::DynamicArray<ComponentSlot> slots_;
		ve::core::Index liveComponentCount_ = 0;
	};
}
