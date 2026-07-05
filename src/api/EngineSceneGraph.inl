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

		[[nodiscard]] EntityId NextSceneEntityId(const std::vector<Entity>& entities) noexcept
		{
			for (std::uint32_t candidate_id = 1; candidate_id != 0; ++candidate_id)
			{
				const bool candidate_is_used = std::ranges::any_of(entities,
					[candidate_id](const Entity& entity) noexcept {
						return entity.id.value == candidate_id;
					});
				if (!candidate_is_used)
				{
					return EntityId{ candidate_id };
				}
			}
			return InvalidEntityId;
		}
	}

	MaterialLibrary& MaterialLibrary::Add(Material material)
	{
		materials.push_back(std::move(material));
		return *this;
	}

	Transform Transform::At(Vec3 position) noexcept
	{
		Transform transform{};
		transform.position = position;
		return transform;
	}

	Entity Entity::Named(std::string name)
	{
		Entity entity{};
		entity.name = std::move(name);
		return entity;
	}

	Entity& Entity::At(Vec3 position) noexcept
	{
		transform.position = position;
		return *this;
	}

	Entity& Entity::Rotate(Vec3 euler_degrees) noexcept
	{
		transform.rotation = euler_degrees;
		return *this;
	}

	Entity& Entity::Scale(Vec3 value) noexcept
	{
		transform.scale = value;
		return *this;
	}

	Entity& Entity::Model(std::string asset_name)
	{
		model = std::move(asset_name);
		return *this;
	}

	Entity& Entity::Material(std::string material_name)
	{
		material = std::move(material_name);
		return *this;
	}

	Entity& Entity::Visible(bool enabled) noexcept
	{
		visible = enabled;
		return *this;
	}

	Entity& Entity::WithId(EntityId entity_id) noexcept
	{
		id = entity_id;
		return *this;
	}

	Entity& Entity::ChildOf(EntityId parent_id) noexcept
	{
		parent = parent_id;
		return *this;
	}

	Entity& Entity::AsRoot() noexcept
	{
		parent = InvalidEntityId;
		return *this;
	}

	SceneGraph& SceneGraph::Add(Entity entity)
	{
		(void)AddEntity(std::move(entity));
		return *this;
	}

	EntityId SceneGraph::AddEntity(Entity entity)
	{
		if (!entity.id.IsValid())
		{
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
		const auto entity = std::ranges::find_if(entities,
			[entity_id](const Entity& candidate) noexcept {
				return candidate.id == entity_id;
			});
		return entity == entities.end() ? nullptr : &*entity;
	}

	const Entity* SceneGraph::FindEntity(EntityId entity_id) const noexcept
	{
		if (!entity_id.IsValid())
		{
			return nullptr;
		}
		const auto entity = std::ranges::find_if(entities,
			[entity_id](const Entity& candidate) noexcept {
				return candidate.id == entity_id;
			});
		return entity == entities.end() ? nullptr : &*entity;
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

		return issues;
	}
}
