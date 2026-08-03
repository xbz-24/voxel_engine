		{
			const auto existing_pool = component_pools_.find(std::type_index(typeid(Component)));
			if (existing_pool == component_pools_.end()) return nullptr;
			return TypedComponentPool<Component>(existing_pool->second.get());
		}

		template <typename Component>
		[[nodiscard]] static ComponentPool<Component>& TypedComponentPool(IComponentPool& pool) noexcept
		{
			return static_cast<ComponentPool<Component>&>(pool);
		}

		template <typename Component>
		[[nodiscard]] static ComponentPool<Component>* TypedComponentPool(IComponentPool* pool) noexcept
		{
			return static_cast<ComponentPool<Component>*>(pool);
		}

		template <typename Component>
		[[nodiscard]] static const ComponentPool<Component>* TypedComponentPool(const IComponentPool* pool) noexcept
		{
			return static_cast<const ComponentPool<Component>*>(pool);
		}

		EntityRegistry registry_;
		std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> component_pools_;
	};
}
