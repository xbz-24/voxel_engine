#pragma once

#include "voxel/Engine.h"

namespace voxel::detail
{
	[[nodiscard]] EntityId NextSceneEntityId(const std::vector<Entity>& entities) noexcept;
	[[nodiscard]] Entity* FindSceneEntity(
		std::vector<Entity>& entities,
		EntityId requested_entity_id) noexcept;
	[[nodiscard]] const Entity* FindSceneEntity(
		const std::vector<Entity>& entities,
		EntityId requested_entity_id) noexcept;
	void ValidateEntityParentCycles(const std::vector<Entity>& entities,
		std::vector<std::string>& issues);
	[[nodiscard]] std::vector<std::string> ValidateSceneEntities(
		const std::vector<Entity>& entities);
}
