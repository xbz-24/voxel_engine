	namespace
	{
		[[nodiscard]] std::string SceneEntityLabel(const Entity& entity)
		{
			if (entity.name.empty())
			{
				return "scene entity";
			}
			return "scene entity '" + entity.name + "'";
		}

		[[nodiscard]] bool ContainsEntityId(const std::vector<Entity>& entities,
			EntityId requested_entity_id) noexcept
		{
			return std::ranges::any_of(entities,
				[requested_entity_id](const Entity& entity) noexcept {
					return entity.id == requested_entity_id;
				});
		}

		[[nodiscard]] EntityId NextSceneEntityId(const std::vector<Entity>& entities) noexcept
		{
			for (std::uint32_t candidate_id = 1; candidate_id != 0; ++candidate_id)
			{
				const EntityId candidate_entity_id{ candidate_id };
				if (!ContainsEntityId(entities, candidate_entity_id))
				{
					return candidate_entity_id;
				}
			}
			return InvalidEntityId;
		}

		template <typename EntityRange>
		[[nodiscard]] auto FindEntityById(EntityRange& entities, EntityId requested_entity_id) noexcept
		{
			return std::ranges::find_if(entities,
				[requested_entity_id](const Entity& candidate_entity) noexcept {
					return candidate_entity.id == requested_entity_id;
				});
		}

		[[nodiscard]] std::set<std::uint32_t> CollectValidEntityIds(
			const std::vector<Entity>& entities,
			std::vector<std::string>& issues)
		{
			std::set<std::uint32_t> entity_ids;
			for (const Entity& entity : entities)
			{
				if (!entity.id.IsValid())
				{
					issues.push_back(SceneEntityLabel(entity) + " must have a stable id");
					continue;
				}
				if (!entity_ids.insert(entity.id.value).second)
				{
					issues.push_back(SceneEntityLabel(entity) + " has a duplicated id");
				}
			}
			return entity_ids;
		}

		void ValidateEntityParents(
			const std::vector<Entity>& entities,
			const std::set<std::uint32_t>& entity_ids,
			std::vector<std::string>& issues)
		{
			for (const Entity& entity : entities)
			{
				if (!entity.id.IsValid() || !entity.parent.IsValid())
				{
					continue;
				}
				if (entity.parent == entity.id)
				{
					issues.push_back(SceneEntityLabel(entity) + " cannot be parented to itself");
					continue;
				}
				if (!entity_ids.contains(entity.parent.value))
				{
					issues.push_back(SceneEntityLabel(entity) + " references a missing parent id");
				}
			}
		}
	}

	MaterialLibrary& MaterialLibrary::Add(Material material)
	{
