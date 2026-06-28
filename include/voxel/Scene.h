#pragma once

#include "voxel/Camera.h"
#include "voxel/Materials.h"

#include <string>
#include <vector>

namespace voxel
{
	// TODO: Add parent/child hierarchy and stable entity ids before SceneGraph can support complex tools.
	struct Transform
	{
		Vec3 position{};
		Vec3 rotation{};
		Vec3 scale{ 1.0f, 1.0f, 1.0f };

		[[nodiscard]] static Transform At(Vec3 position) noexcept;
	};

	struct Entity
	{
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
	};

	struct SceneGraph
	{
		// TODO: Define how SceneGraph entities coexist with voxel world edits and renderer-owned chunk meshes.
		std::vector<Entity> entities;
		std::vector<Light> lights;
		Environment environment{};

		SceneGraph& Add(Entity entity);
		SceneGraph& Add(Light light);
		SceneGraph& EnvironmentSettings(Environment value) noexcept;
	};
}
