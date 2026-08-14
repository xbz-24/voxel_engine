#include "EngineSceneGraphHelpers.h"

#include <cstdint>
#include <set>
#include <string>

namespace voxel::detail
{
	namespace
	{
		[[nodiscard]] std::string SceneEntityLabel(const Entity& entity)
		{
			return entity.name.empty() ? "scene entity" : "scene entity '" + entity.name + "'";
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

		void ValidateEntityParents(const std::vector<Entity>& entities,
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

	std::vector<std::string> ValidateSceneEntities(const std::vector<Entity>& entities)
	{
		std::vector<std::string> issues;
		const std::set<std::uint32_t> entity_ids = CollectValidEntityIds(entities, issues);
		ValidateEntityParents(entities, entity_ids, issues);
		ValidateEntityParentCycles(entities, issues);
		return issues;
	}
}
