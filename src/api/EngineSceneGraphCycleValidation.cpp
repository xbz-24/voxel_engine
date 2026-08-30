#include "EngineSceneGraphHelpers.h"

#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace voxel::detail
{
	namespace
	{
		enum class VisitState { Unvisited, Visiting, Done };
		using EntityIndex = std::map<std::uint32_t, const Entity*>;

		[[nodiscard]] EntityIndex IndexUniqueEntities(const std::vector<Entity>& entities)
		{
			EntityIndex index;
			std::set<std::uint32_t> duplicate_ids;
			for (const Entity& entity : entities)
			{
				if (!entity.id.IsValid() || duplicate_ids.contains(entity.id.value)) continue;
				const auto [position, inserted] = index.emplace(entity.id.value, &entity);
				if (inserted) continue;
				index.erase(position);
				duplicate_ids.insert(entity.id.value);
			}
			return index;
		}

		[[nodiscard]] std::string CycleIssue(const Entity& entity)
		{
			const std::string label = entity.name.empty() ?
				"scene entity" : "scene entity '" + entity.name + "'";
			return label + " participates in a parent cycle";
		}
	}

	void ValidateEntityParentCycles(const std::vector<Entity>& entities,
		std::vector<std::string>& issues)
	{
		const EntityIndex index = IndexUniqueEntities(entities);
		std::map<std::uint32_t, VisitState> states;
		std::set<std::uint32_t> cycle_member_ids;
		for (const auto& [origin_id, unused_entity] : index)
		{
			static_cast<void>(unused_entity);
			if (states[origin_id] == VisitState::Done) continue;
			std::vector<std::uint32_t> path;
			std::uint32_t current_id = origin_id;
			while (true)
			{
				VisitState& state = states[current_id];
				if (state == VisitState::Done) break;
				if (state == VisitState::Visiting)
				{
					const auto cycle_begin = std::ranges::find(path, current_id);
					cycle_member_ids.insert(cycle_begin, path.end());
					break;
				}
				state = VisitState::Visiting;
				path.push_back(current_id);
				const Entity& entity = *index.at(current_id);
				if (!entity.parent.IsValid() || entity.parent == entity.id ||
					!index.contains(entity.parent.value)) break;
				current_id = entity.parent.value;
			}
			for (std::uint32_t visited_id : path) states[visited_id] = VisitState::Done;
		}
		for (std::uint32_t member_id : cycle_member_ids)
		{
			issues.push_back(CycleIssue(*index.at(member_id)));
		}
	}
}
