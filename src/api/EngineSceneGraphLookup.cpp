#include "EngineSceneGraphHelpers.h"

#include <algorithm>
#include <cstdint>

namespace voxel::detail
{
	namespace
	{
		[[nodiscard]] bool ContainsEntityId(const std::vector<Entity>& entities,
			EntityId requested_entity_id) noexcept
		{
			return std::ranges::any_of(entities,
				[requested_entity_id](const Entity& entity) noexcept {
					return entity.id == requested_entity_id;
				});
		}
	}

	EntityId NextSceneEntityId(const std::vector<Entity>& entities) noexcept
	{
		for (std::uint32_t candidate_id = 1; candidate_id != 0; ++candidate_id)
		{
			const EntityId candidate{ candidate_id };
			if (!ContainsEntityId(entities, candidate))
			{
				return candidate;
			}
		}
		return InvalidEntityId;
	}

	Entity* FindSceneEntity(std::vector<Entity>& entities, EntityId requested_entity_id) noexcept
	{
		const auto iterator = std::ranges::find_if(entities,
			[requested_entity_id](const Entity& candidate) noexcept {
				return candidate.id == requested_entity_id;
			});
		return iterator == entities.end() ? nullptr : &*iterator;
	}

	const Entity* FindSceneEntity(const std::vector<Entity>& entities,
		EntityId requested_entity_id) noexcept
	{
		const auto iterator = std::ranges::find_if(entities,
			[requested_entity_id](const Entity& candidate) noexcept {
				return candidate.id == requested_entity_id;
			});
		return iterator == entities.end() ? nullptr : &*iterator;
	}
}
