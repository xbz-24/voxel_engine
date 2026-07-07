#pragma once

#include "voxel/Camera.h"
#include "voxel/Materials.h"

#include <cstdint>
#include <string>
#include <vector>

namespace voxel
{
	struct EntityId
	{
		std::uint32_t value = 0;

		[[nodiscard]] constexpr bool IsValid() const noexcept
		{
			return value != 0;
		}

		[[nodiscard]] friend constexpr bool operator==(const EntityId&, const EntityId&) noexcept = default;
	};

	inline constexpr EntityId InvalidEntityId{};

	struct Transform
	{
		Vec3 position{};
		Vec3 rotation{};
		Vec3 scale{ 1.0f, 1.0f, 1.0f };

		[[nodiscard]] static Transform At(Vec3 position) noexcept;
	};

	struct Entity
	{
		EntityId id = InvalidEntityId;
		EntityId parent = InvalidEntityId;
		std::string name;
		Transform transform{};
		std::string model;
		std::string material;
		bool visible = true;

		[[nodiscard]] static Entity Named(std::string name);

		Entity& At(Vec3 position) noexcept;
		Entity& Rotate(Vec3 euler_degrees) noexcept;
		Entity& Scale(Vec3 value) noexcept;
		Entity& Model(std::string asset_name);
		Entity& Material(std::string material_name);
		Entity& Visible(bool enabled = true) noexcept;
		Entity& WithId(EntityId entity_id) noexcept;
		Entity& ChildOf(EntityId parent_id) noexcept;
		Entity& AsRoot() noexcept;
	};

	enum class SceneGraphCompositionMode
	{
		IndependentOverlay,
		VoxelWorldAnchored
	};

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
}
