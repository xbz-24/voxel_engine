			entity.id = NextSceneEntityId(entities);
		}

		const EntityId entity_id = entity.id;
		entities.push_back(std::move(entity));
		return entity_id;
	}

	EntityId SceneGraph::AddChild(EntityId parent_id, Entity entity)
	{
		entity.ChildOf(parent_id);
		return AddEntity(std::move(entity));
	}

	Entity* SceneGraph::FindEntity(EntityId entity_id) noexcept
	{
		if (!entity_id.IsValid())
		{
			return nullptr;
		}
		const auto entity_iterator = FindEntityById(entities, entity_id);
		return entity_iterator == entities.end() ? nullptr : &*entity_iterator;
	}

	const Entity* SceneGraph::FindEntity(EntityId entity_id) const noexcept
	{
		if (!entity_id.IsValid())
		{
			return nullptr;
		}
		const auto entity_iterator = FindEntityById(entities, entity_id);
		return entity_iterator == entities.end() ? nullptr : &*entity_iterator;
	}

	SceneGraph& SceneGraph::Add(Light light)
	{
		lights.push_back(light);
		return *this;
	}

	SceneGraph& SceneGraph::EnvironmentSettings(Environment value) noexcept
	{
		environment = value;
		return *this;
	}

	std::vector<std::string> SceneGraph::Validate() const
	{
		std::vector<std::string> issues;
		const std::set<std::uint32_t> entity_ids = CollectValidEntityIds(entities, issues);
		ValidateEntityParents(entities, entity_ids, issues);
		return issues;
	}
}
