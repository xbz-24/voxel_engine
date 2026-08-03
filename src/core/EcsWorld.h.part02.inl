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
				if (entity_id >= slots_.size()) slots_.resize(ve::core::ToIndex(entity_id) + 1U);
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
				return TypedComponentPool<Component>(*existing_pool->second);
			}
			auto [new_pool, inserted] = component_pools_.emplace(
				component_type,
				std::make_unique<ComponentPool<Component>>());
			(void)inserted;
			return TypedComponentPool<Component>(*new_pool->second);
		}

		template <typename Component>
		[[nodiscard]] ComponentPool<Component>* FindComponentPool() noexcept
		{
			const auto existing_pool = component_pools_.find(std::type_index(typeid(Component)));
			if (existing_pool == component_pools_.end()) return nullptr;
			return TypedComponentPool<Component>(existing_pool->second.get());
		}

		template <typename Component>
		[[nodiscard]] const ComponentPool<Component>* FindComponentPool() const noexcept
