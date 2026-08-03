
#include "voxel/Camera.h"
#include "voxel/Materials.h"

#include <cstdint>
#include <string>
#include <vector>

namespace voxel
{
	/** Stable public identifier for scene graph entities. */
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

	/** Local transform used by public scene entities. */
	struct Transform
	{
		Vec3 position{};
		Vec3 rotation{};
		Vec3 scale{ 1.0f, 1.0f, 1.0f };

		/** Creates a transform located at position. */
		[[nodiscard]] static Transform At(Vec3 position) noexcept;
	};

	/** Public scene entity with optional model and material bindings. */
	struct Entity
	{
		EntityId id = InvalidEntityId;
		EntityId parent = InvalidEntityId;
		std::string name;
		Transform transform{};
		std::string model;
		std::string material;
		bool visible = true;

		/** Creates a named entity. */
		[[nodiscard]] static Entity Named(std::string name);

		/** Sets entity position. */
		Entity& At(Vec3 position) noexcept;

		/** Sets entity rotation in Euler degrees. */
		Entity& Rotate(Vec3 euler_degrees) noexcept;

		/** Sets entity scale. */
		Entity& Scale(Vec3 value) noexcept;

		/** Binds a model asset by name. */
		Entity& Model(std::string asset_name);

		/** Binds a material by name. */
		Entity& Material(std::string material_name);

		/** Sets entity visibility. */
		Entity& Visible(bool enabled = true) noexcept;

		/** Assigns a specific public entity id. */
		Entity& WithId(EntityId entity_id) noexcept;

		/** Parents this entity to another entity id. */
		Entity& ChildOf(EntityId parent_id) noexcept;

		/** Clears this entity's parent. */
		Entity& AsRoot() noexcept;
	};

	/** Defines how authored entities are composed with the voxel world. */
	enum class SceneGraphCompositionMode
	{
		IndependentOverlay,
		VoxelWorldAnchored
	};

	/** Public authored scene graph for non-voxel entities, lights, and environment. */
	struct SceneGraph
	{
