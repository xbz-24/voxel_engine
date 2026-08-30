#pragma once

#include "voxel/Environment.h"
#include "voxel/Light.h"
#include "voxel/SceneEntity.h"

#include <string>
#include <vector>

namespace voxel
{
	/** @addtogroup voxel_sdk_scene_graph
	 * @{
	 */
	enum class SceneGraphCompositionMode
	{
		IndependentOverlay,
		VoxelWorldAnchored
	};

	/** Public authored scene graph for non-voxel entities, lights, and environment. */
	struct SceneGraph
	{
		SceneGraphCompositionMode composition_mode = SceneGraphCompositionMode::IndependentOverlay;
		std::vector<Entity> entities;
		std::vector<Light> lights;
		Environment environment{};

		SceneGraph& ComposeAs(SceneGraphCompositionMode mode) noexcept;
		SceneGraph& Add(Entity entity);
		[[nodiscard]] EntityId AddEntity(Entity entity);
		[[nodiscard]] EntityId AddChild(EntityId parent_id, Entity entity);
		[[nodiscard]] Entity* FindEntity(EntityId entity_id) noexcept;
		[[nodiscard]] const Entity* FindEntity(EntityId entity_id) const noexcept;
		SceneGraph& Add(Light light);
		SceneGraph& EnvironmentSettings(Environment value) noexcept;
		[[nodiscard]] std::vector<std::string> Validate() const;
	};
	/** @} */
}
