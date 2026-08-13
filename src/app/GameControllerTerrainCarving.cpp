#include "GameControllerHelpers.h"

#include "BlockRegistry.h"
#include "CoreTypes.h"
#include "World.h"

#include <algorithm>
#include <array>

namespace ve::engine::game_controller_detail
{
	bool IsTunnelMaterial(ve::blocks::BlockId block) noexcept
	{
		using enum ve::blocks::BlockId;
		static constexpr std::array<ve::blocks::BlockId, 16> materials{ {
			Grass, Dirt, Stone, Cobblestone, Gravel, MossBlock, MossyCobblestone, Granite,
			Andesite, Diorite, Deepslate, Sand, Sandstone, RedSand, RedSandstone, Terracotta
		} };
		return std::ranges::find(materials, block) != materials.end();
	}

	namespace
	{
		[[nodiscard]] bool SetBlockIfChanged(
			ve::world::World& world,
			const glm::ivec3& block_position,
			ve::blocks::BlockId block)
		{
			return world.GetBlock(block_position) != block && world.SetBlock(block_position, block);
		}
	}

	bool CarveRoundedHole(
		ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		const glm::ivec3& tunnel_center)
	{
		bool changed = false;
		constexpr int tunnel_radius = 3;
		constexpr int tunnel_depth = 6;
		for (int x = -tunnel_radius; x <= tunnel_radius; ++x)
		{
			for (int z = -tunnel_radius; z <= tunnel_radius; ++z)
			{
				const float horizontal = ve::core::ToFloat((x * x) + (z * z)) /
					ve::core::ToFloat(tunnel_radius * tunnel_radius);
				if (horizontal > 1.10f) continue;
				for (int y = 0; y >= -tunnel_depth; --y)
				{
					const float vertical = ve::core::ToFloat(y * y) /
						ve::core::ToFloat(tunnel_depth * tunnel_depth);
					if (horizontal + (vertical * 0.72f) > 1.0f) continue;
					const glm::ivec3 position = tunnel_center + glm::ivec3{ x, y, z };
					const auto block = world.GetBlock(position);
					if (!block_registry.IsSolid(block) || !IsTunnelMaterial(block)) continue;
					changed = SetBlockIfChanged(world, position, ve::blocks::BlockId::Air) || changed;
				}
			}
		}

		for (int x = -(tunnel_radius + 1); x <= tunnel_radius + 1; ++x)
		{
			for (int z = -(tunnel_radius + 1); z <= tunnel_radius + 1; ++z)
			{
				const int distance_squared = (x * x) + (z * z);
				if (distance_squared < tunnel_radius * tunnel_radius ||
					distance_squared > (tunnel_radius + 1) * (tunnel_radius + 1)) continue;
				const glm::ivec3 position = tunnel_center + glm::ivec3{ x, 0, z };
				if (!IsTunnelMaterial(world.GetBlock(position))) continue;
				const auto rim = ((x + z) & 1) == 0
					? ve::blocks::BlockId::MossBlock
					: ve::blocks::BlockId::MossyCobblestone;
				changed = SetBlockIfChanged(world, position, rim) || changed;
			}
		}
		return changed;
	}
}
