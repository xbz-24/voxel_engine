#include "voxel/Engine.h"

#include "EngineSceneGraphHelpers.h"

#include <utility>

namespace voxel
{
	MaterialLibrary& MaterialLibrary::Add(Material material)
	{
		materials.push_back(std::move(material));
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
			entity.id = detail::NextSceneEntityId(entities);
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
		return detail::FindSceneEntity(entities, entity_id);
	}

	const Entity* SceneGraph::FindEntity(EntityId entity_id) const noexcept
	{
		if (!entity_id.IsValid())
		{
			return nullptr;
		}
		return detail::FindSceneEntity(entities, entity_id);
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
		return detail::ValidateSceneEntities(entities);
	}
}
