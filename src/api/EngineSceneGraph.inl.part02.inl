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

	SceneGraph& SceneGraph::ComposeAs(SceneGraphCompositionMode mode) noexcept
	{
		composition_mode = mode;
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
