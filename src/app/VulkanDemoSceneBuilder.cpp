#include "VulkanDemoSceneBuilder.h"

#include "Block.h"
#include "Chunk.h"
#include "ChunkTerrain.h"
#include "World.h"

#include <algorithm>
#include <array>
#include <boost/container/small_vector.hpp>
#include <cmath>
#include <cstdint>
#include <glm/vec3.hpp>
#include <random>

namespace ve::engine
{
	namespace
	{
		using ve::blocks::BlockId;
		using ve::rendering::VulkanMinecraftDemoPreset;
		using ve::rendering::VulkanMinecraftDemoSceneConfig;

		struct DemoBounds
		{
			int width = 0;
			int depth = 0;
			int height = ve::world::terrain::ChunkHeight;
			int center_x = 0;
			int center_z = 0;
		};

		struct TreeStyle
		{
			BlockId log;
			BlockId leaves;
			int trunk_bonus;
		};

		[[nodiscard]] VulkanMinecraftDemoSceneConfig Sanitize(VulkanMinecraftDemoSceneConfig config) noexcept
		{
			config.ground_y = std::clamp(config.ground_y, 34, 76);
			config.terrain_radius = std::clamp(config.terrain_radius, 24, 62);
			config.hill_height = std::clamp(config.hill_height, 0, 28);
			config.house_count = std::clamp(config.house_count, 0, 9);
			config.tree_count = std::clamp(config.tree_count, 0, 96);
			config.farm_rows = std::clamp(config.farm_rows, 0, 16);
			config.water_radius = std::clamp(config.water_radius, 0, 22);
			config.tower_height = std::clamp(config.tower_height, 6, 28);
			config.lantern_count = std::clamp(config.lantern_count, 0, 32);
			config.ore_richness = std::clamp(config.ore_richness, 0, 32);
			config.market_stall_count = std::clamp(config.market_stall_count, 0, 8);
			config.floating_island_count = std::clamp(config.floating_island_count, 0, 6);
			config.ruin_count = std::clamp(config.ruin_count, 0, 8);
			return config;
		}

		[[nodiscard]] DemoBounds BoundsFor(const ve::world::World& world) noexcept
		{
			const ve::world::WorldMetrics metrics = world.Metrics();
			const int width = metrics.worldSizeChunks * Chunk::CHUNK_WIDTH;
			const int depth = metrics.worldSizeChunks * Chunk::CHUNK_DEPTH;
			return DemoBounds{ width, depth, ve::world::terrain::ChunkHeight, width / 2, depth / 2 };
		}

		[[nodiscard]] bool IsInside(const DemoBounds& bounds, int x, int y, int z) noexcept
		{
			return x >= 0 && x < bounds.width && y >= 0 && y < bounds.height && z >= 0 && z < bounds.depth;
		}

		void SetBlock(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, BlockId block)
		{
			if (IsInside(bounds, x, y, z)) world.SetBlock(x, y, z, block);
		}

		void FillBox(ve::world::World& world, const DemoBounds& bounds, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z, BlockId block)
		{
			const int begin_x = std::max(0, std::min(min_x, max_x));
			const int begin_y = std::max(0, std::min(min_y, max_y));
			const int begin_z = std::max(0, std::min(min_z, max_z));
			const int end_x = std::min(bounds.width - 1, std::max(min_x, max_x));
			const int end_y = std::min(bounds.height - 1, std::max(min_y, max_y));
			const int end_z = std::min(bounds.depth - 1, std::max(min_z, max_z));
			for (int x = begin_x; x <= end_x; ++x)
			{
				for (int y = begin_y; y <= end_y; ++y)
				{
					for (int z = begin_z; z <= end_z; ++z)
					{
						world.SetBlock(x, y, z, block);
					}
				}
			}
		}

		[[nodiscard]] float Hash01(int x, int z, int seed) noexcept
		{
			std::uint32_t value = static_cast<std::uint32_t>(x) * 0x9E3779B9u;
			value ^= static_cast<std::uint32_t>(z) * 0x85EBCA6Bu;
			value ^= static_cast<std::uint32_t>(seed) * 0xC2B2AE35u;
			value ^= value >> 16u;
			value *= 0x7FEB352Du;
			value ^= value >> 15u;
			return static_cast<float>(value & 0xFFFFu) / 65535.0f;
		}

		[[nodiscard]] int TerrainHeightAt(const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config, int x, int z) noexcept
		{
			const float dx = static_cast<float>(x - bounds.center_x);
			const float dz = static_cast<float>(z - bounds.center_z);
			const float distance = std::sqrt((dx * dx) + (dz * dz));
			const float radial = std::max(0.0f, 1.0f - (distance / static_cast<float>(std::max(config.terrain_radius, 1))));
			const float wave = std::sin((static_cast<float>(x + config.seed) * 0.17f)) +
				std::cos((static_cast<float>(z - config.seed) * 0.13f));
			float height = static_cast<float>(config.ground_y) + (radial * radial * static_cast<float>(config.hill_height) * 0.38f) + (wave * 1.25f);

			if (config.preset == VulkanMinecraftDemoPreset::QuarryOutpost)
			{
				const float ridge = std::max(0.0f, 1.0f - std::abs(static_cast<float>(x - (bounds.center_x + 28))) / 24.0f);
				height += ridge * static_cast<float>(config.hill_height) * 0.65f;
			}
			if (config.preset == VulkanMinecraftDemoPreset::ForestFarm)
			{
				height += Hash01(x / 3, z / 3, config.seed) * 2.5f;
			}
			if (config.preset == VulkanMinecraftDemoPreset::StressTown)
			{
				height += static_cast<float>(((x / 6) + (z / 6)) % 2) * 2.0f;
			}

			if (config.water && config.water_radius > 0)
			{
				const int lake_x = bounds.center_x - 22;
				const int lake_z = bounds.center_z + 28;
				const int lake_dx = x - lake_x;
				const int lake_dz = z - lake_z;
				if ((lake_dx * lake_dx) + (lake_dz * lake_dz) < config.water_radius * config.water_radius)
				{
					height = std::min(height, static_cast<float>(config.ground_y - 2));
				}
			}
			return std::clamp(static_cast<int>(std::round(height)), 6, bounds.height - 8);
		}

		void ClearColumnAboveDemoFloor(ve::world::World& world, const DemoBounds& bounds, int x, int z, int floor_y)
		{
			FillBox(world, bounds, x, std::max(1, floor_y - 12), z, x, bounds.height - 1, z, BlockId::Air);
		}

		void SetTerrainColumn(ve::world::World& world, const DemoBounds& bounds, int x, int z, int surface_y, BlockId surface)
		{
			FillBox(world, bounds, x, std::max(0, surface_y - 7), z, x, surface_y - 4, z, BlockId::Stone);
			FillBox(world, bounds, x, surface_y - 3, z, x, surface_y - 1, z, BlockId::Dirt);
			SetBlock(world, bounds, x, surface_y, z, surface);
		}

		void BuildTerrain(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int margin = 4;
			for (int x = margin; x < bounds.width - margin; ++x)
			{
				for (int z = margin; z < bounds.depth - margin; ++z)
				{
					ClearColumnAboveDemoFloor(world, bounds, x, z, config.ground_y);
					const int surface_y = TerrainHeightAt(bounds, config, x, z);
					BlockId surface = surface_y >= config.ground_y + config.hill_height - 1 && config.hill_height > 14 ? BlockId::Snow : BlockId::Grass;
					if (Hash01(x, z, config.seed + 41) < 0.035f && surface == BlockId::Grass) surface = BlockId::MossBlock;
					SetTerrainColumn(world, bounds, x, z, surface_y, surface);
				}
			}
		}

		void FlattenArea(ve::world::World& world, const DemoBounds& bounds, int min_x, int min_z, int max_x, int max_z, int y, BlockId surface)
		{
			for (int x = min_x; x <= max_x; ++x)
			{
				for (int z = min_z; z <= max_z; ++z)
				{
					FillBox(world, bounds, x, y - 8, z, x, bounds.height - 1, z, BlockId::Air);
					SetTerrainColumn(world, bounds, x, z, y, surface);
				}
			}
		}

		[[nodiscard]] int FindGroundY(const ve::world::World& world, const DemoBounds& bounds, int x, int z, int fallback_y)
		{
			for (int y = std::min(bounds.height - 2, fallback_y + 34); y >= 1; --y)
			{
				const BlockId block = world.GetBlock(x, y, z);
				if (block != BlockId::Air && block != BlockId::Water && block != BlockId::Glass) return y;
			}
			return fallback_y;
		}

		void BuildPathRect(ve::world::World& world, const DemoBounds& bounds, int min_x, int min_z, int max_x, int max_z, int y)
		{
			FillBox(world, bounds, min_x, y, min_z, max_x, y, max_z, BlockId::Gravel);
			if ((max_x - min_x) > (max_z - min_z))
			{
				FillBox(world, bounds, min_x, y, min_z - 1, max_x, y, min_z - 1, BlockId::MossyCobblestone);
				FillBox(world, bounds, min_x, y, max_z + 1, max_x, y, max_z + 1, BlockId::MossyCobblestone);
			}
			else
			{
				FillBox(world, bounds, min_x - 1, y, min_z, min_x - 1, y, max_z, BlockId::MossyCobblestone);
				FillBox(world, bounds, max_x + 1, y, min_z, max_x + 1, y, max_z, BlockId::MossyCobblestone);
			}
		}

		void BuildHouse(ve::world::World& world, const DemoBounds& bounds, int x, int z, int y, int style)
		{
			const std::array wood_blocks{ BlockId::OakPlanks, BlockId::BirchPlanks, BlockId::SprucePlanks, BlockId::CherryPlanks };
			const std::array log_blocks{ BlockId::OakLog, BlockId::BirchLog, BlockId::SpruceLog, BlockId::CherryLog };
			const BlockId wall = wood_blocks[static_cast<std::size_t>(style) % wood_blocks.size()];
			const BlockId log = log_blocks[static_cast<std::size_t>(style) % log_blocks.size()];
			const BlockId roof = style % 3 == 0 ? BlockId::Bricks : (style % 3 == 1 ? BlockId::Cobblestone : BlockId::Blackstone);
			const int width = 8 + (style % 2) * 2;
			const int depth = 8;
			FlattenArea(world, bounds, x - 1, z - 1, x + width, z + depth, y, BlockId::Grass);
			FillBox(world, bounds, x, y, z, x + width - 1, y, z + depth - 1, BlockId::OakPlanks);

			for (int level = 1; level <= 4; ++level)
			{
				FillBox(world, bounds, x, y + level, z, x + width - 1, y + level, z, wall);
				FillBox(world, bounds, x, y + level, z + depth - 1, x + width - 1, y + level, z + depth - 1, wall);
				FillBox(world, bounds, x, y + level, z, x, y + level, z + depth - 1, wall);
				FillBox(world, bounds, x + width - 1, y + level, z, x + width - 1, y + level, z + depth - 1, wall);
			}
			FillBox(world, bounds, x, y + 1, z, x, y + 5, z, log);
			FillBox(world, bounds, x + width - 1, y + 1, z, x + width - 1, y + 5, z, log);
			FillBox(world, bounds, x, y + 1, z + depth - 1, x, y + 5, z + depth - 1, log);
			FillBox(world, bounds, x + width - 1, y + 1, z + depth - 1, x + width - 1, y + 5, z + depth - 1, log);

			FillBox(world, bounds, x - 1, y + 5, z - 1, x + width, y + 5, z + depth, roof);
			FillBox(world, bounds, x, y + 6, z, x + width - 1, y + 6, z + depth - 1, roof);
			FillBox(world, bounds, x + 2, y + 7, z + 2, x + width - 3, y + 7, z + depth - 3, roof);
			FillBox(world, bounds, x + 3, y + 8, z + 3, x + width - 4, y + 8, z + depth - 4, roof);

			FillBox(world, bounds, x + width / 2 - 1, y + 1, z, x + width / 2, y + 3, z, BlockId::Air);
			SetBlock(world, bounds, x + 2, y + 2, z, BlockId::Glass);
			SetBlock(world, bounds, x + width - 3, y + 2, z + depth - 1, BlockId::Glass);
			SetBlock(world, bounds, x, y + 2, z + 3, BlockId::Glass);
			SetBlock(world, bounds, x + width - 1, y + 2, z + 4, BlockId::Glass);
			SetBlock(world, bounds, x + 2, y + 1, z + 2, BlockId::CraftingTable);
			SetBlock(world, bounds, x + 3, y + 1, z + 2, BlockId::Bookshelf);
			SetBlock(world, bounds, x + 4, y + 1, z + 2, BlockId::Bookshelf);
		}

		void BuildWell(ve::world::World& world, const DemoBounds& bounds, int x, int z, int y)
		{
			FillBox(world, bounds, x - 2, y, z - 2, x + 2, y, z + 2, BlockId::MossyCobblestone);
			FillBox(world, bounds, x - 1, y + 1, z - 1, x + 1, y + 1, z + 1, BlockId::Water);
			FillBox(world, bounds, x - 2, y + 1, z - 2, x - 2, y + 4, z - 2, BlockId::OakLog);
			FillBox(world, bounds, x + 2, y + 1, z - 2, x + 2, y + 4, z - 2, BlockId::OakLog);
			FillBox(world, bounds, x - 2, y + 1, z + 2, x - 2, y + 4, z + 2, BlockId::OakLog);
			FillBox(world, bounds, x + 2, y + 1, z + 2, x + 2, y + 4, z + 2, BlockId::OakLog);
			FillBox(world, bounds, x - 2, y + 5, z - 2, x + 2, y + 5, z + 2, BlockId::Bricks);
			SetBlock(world, bounds, x, y + 4, z, BlockId::SeaLantern);
		}

		void BuildVillage(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.village) return;
			const int y = config.ground_y;
			const int center_x = bounds.center_x + 2;
			const int center_z = bounds.center_z + 2;
			FlattenArea(world, bounds, center_x - 28, center_z - 22, center_x + 32, center_z + 24, y, BlockId::Grass);
			BuildPathRect(world, bounds, center_x - 31, center_z - 2, center_x + 35, center_z + 2, y);
			BuildPathRect(world, bounds, center_x - 2, center_z - 24, center_x + 2, center_z + 26, y);
			BuildWell(world, bounds, center_x, center_z, y);

			boost::container::small_vector<glm::ivec3, 9> lots{
				glm::ivec3{ center_x - 24, y, center_z - 18 },
				glm::ivec3{ center_x + 12, y, center_z - 18 },
				glm::ivec3{ center_x - 25, y, center_z + 10 },
				glm::ivec3{ center_x + 15, y, center_z + 10 },
				glm::ivec3{ center_x - 6, y, center_z - 33 },
				glm::ivec3{ center_x + 34, y, center_z - 4 },
				glm::ivec3{ center_x - 39, y, center_z - 3 },
				glm::ivec3{ center_x + 2, y, center_z + 30 },
				glm::ivec3{ center_x - 44, y, center_z + 20 }
			};
			for (int index = 0; index < std::min<int>(config.house_count, static_cast<int>(lots.size())); ++index)
			{
				BuildHouse(world, bounds, lots[static_cast<std::size_t>(index)].x, lots[static_cast<std::size_t>(index)].z, y, index);
			}
		}

		void BuildLake(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.water || config.water_radius <= 0) return;
			const int lake_x = bounds.center_x - 22;
			const int lake_z = bounds.center_z + 28;
			const int radius = config.water_radius;
			for (int x = lake_x - radius - 3; x <= lake_x + radius + 3; ++x)
			{
				for (int z = lake_z - radius - 3; z <= lake_z + radius + 3; ++z)
				{
					const int dx = x - lake_x;
					const int dz = z - lake_z;
					const int distance_sq = (dx * dx) + (dz * dz);
					if (distance_sq > (radius + 2) * (radius + 2)) continue;
					const bool water = distance_sq <= radius * radius;
					FillBox(world, bounds, x, config.ground_y - 5, z, x, config.ground_y + 2, z, BlockId::Air);
					SetTerrainColumn(world, bounds, x, z, config.ground_y - (water ? 2 : 1), water ? BlockId::Sand : BlockId::Grass);
					if (water) SetBlock(world, bounds, x, config.ground_y - 1, z, BlockId::Water);
					if (!water && Hash01(x, z, config.seed + 73) < 0.35f) SetBlock(world, bounds, x, config.ground_y, z, BlockId::Sand);
				}
			}
			FillBox(world, bounds, lake_x - radius - 2, config.ground_y, lake_z - 1, lake_x + radius + 2, config.ground_y, lake_z + 1, BlockId::OakPlanks);
			FillBox(world, bounds, lake_x - radius - 2, config.ground_y + 1, lake_z - 2, lake_x + radius + 2, config.ground_y + 1, lake_z - 2, BlockId::OakLog);
			FillBox(world, bounds, lake_x - radius - 2, config.ground_y + 1, lake_z + 2, lake_x + radius + 2, config.ground_y + 1, lake_z + 2, BlockId::OakLog);
		}

		void BuildFarm(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.farms || config.farm_rows <= 0) return;
			const int y = config.ground_y;
			const int min_x = bounds.center_x - 46;
			const int min_z = bounds.center_z - 18;
			const int rows = config.farm_rows;
			FlattenArea(world, bounds, min_x - 2, min_z - 2, min_x + 24, min_z + rows + 4, y, BlockId::Dirt);
			for (int row = 0; row < rows; ++row)
			{
				const BlockId crop = row % 3 == 0 ? BlockId::HayBlock : (row % 3 == 1 ? BlockId::Pumpkin : BlockId::Melon);
				for (int x = min_x; x <= min_x + 22; ++x)
				{
					SetBlock(world, bounds, x, y, min_z + row, row % 4 == 0 ? BlockId::Water : BlockId::Dirt);
					if (row % 4 != 0 && x % 3 == 0) SetBlock(world, bounds, x, y + 1, min_z + row, crop);
				}
			}
			FillBox(world, bounds, min_x - 1, y, min_z - 1, min_x + 23, y, min_z - 1, BlockId::OakLog);
			FillBox(world, bounds, min_x - 1, y, min_z + rows + 1, min_x + 23, y, min_z + rows + 1, BlockId::OakLog);
		}

		void BuildQuarryAndCave(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.cave) return;
			const int qx = bounds.center_x + 31;
			const int qz = bounds.center_z + 30;
			const int y = config.ground_y;
			for (int step = 0; step < 5; ++step)
			{
				FillBox(world, bounds, qx - 11 + step, y + step, qz - 9 + step, qx + 11 - step, y + step, qz + 9 - step,
					step % 2 == 0 ? BlockId::Stone : BlockId::Deepslate);
			}
			FillBox(world, bounds, qx - 6, y + 1, qz - 10, qx + 6, y + 6, qz - 4, BlockId::Air);
			FillBox(world, bounds, qx - 7, y + 1, qz - 11, qx - 7, y + 6, qz - 4, BlockId::Blackstone);
			FillBox(world, bounds, qx + 7, y + 1, qz - 11, qx + 7, y + 6, qz - 4, BlockId::Blackstone);
			FillBox(world, bounds, qx - 7, y + 7, qz - 11, qx + 7, y + 7, qz - 4, BlockId::Basalt);
			const std::array ores{ BlockId::CoalOre, BlockId::IronOre, BlockId::CopperOre, BlockId::GoldOre, BlockId::EmeraldOre, BlockId::LapisOre, BlockId::DiamondOre };
			for (int index = 0; index < config.ore_richness; ++index)
			{
				const int ox = qx - 8 + ((index * 5 + config.seed) % 17);
				const int oy = y + 1 + ((index * 7) % 6);
				const int oz = qz - 11 + ((index * 3) % 8);
				SetBlock(world, bounds, ox, oy, oz, ores[static_cast<std::size_t>(index) % ores.size()]);
			}
			SetBlock(world, bounds, qx, y + 2, qz - 5, BlockId::SeaLantern);
			SetBlock(world, bounds, qx - 3, y + 1, qz - 7, BlockId::AmethystBlock);
			SetBlock(world, bounds, qx + 3, y + 1, qz - 7, BlockId::AmethystBlock);
		}

		void BuildWatchtower(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int x = bounds.center_x + 38;
			const int z = bounds.center_z - 21;
			const int y = config.ground_y;
			const int top = y + config.tower_height;
			FlattenArea(world, bounds, x - 5, z - 5, x + 5, z + 5, y, BlockId::Grass);
			FillBox(world, bounds, x - 1, y + 1, z - 1, x + 1, top - 3, z + 1, BlockId::OakLog);
			FillBox(world, bounds, x - 4, top - 4, z - 4, x + 4, top - 4, z + 4, BlockId::SprucePlanks);
			FillBox(world, bounds, x - 4, top - 3, z - 4, x - 4, top - 1, z + 4, BlockId::SprucePlanks);
			FillBox(world, bounds, x + 4, top - 3, z - 4, x + 4, top - 1, z + 4, BlockId::SprucePlanks);
			FillBox(world, bounds, x - 4, top - 3, z - 4, x + 4, top - 1, z - 4, BlockId::SprucePlanks);
			FillBox(world, bounds, x - 4, top - 3, z + 4, x + 4, top - 1, z + 4, BlockId::SprucePlanks);
			FillBox(world, bounds, x - 3, top, z - 3, x + 3, top, z + 3, BlockId::Bricks);
			SetBlock(world, bounds, x, top + 1, z, BlockId::SeaLantern);
			for (int step = 0; step < config.tower_height - 5; step += 2)
			{
				SetBlock(world, bounds, x - 2, y + 1 + step, z, BlockId::OakPlanks);
				SetBlock(world, bounds, x + 2, y + 2 + step, z, BlockId::OakPlanks);
			}
		}

		void PlaceTree(ve::world::World& world, const DemoBounds& bounds, int x, int z, int y, const TreeStyle& style)
		{
			const int trunk_height = 4 + style.trunk_bonus + static_cast<int>(Hash01(x, z, y) * 3.0f);
			FillBox(world, bounds, x, y + 1, z, x, y + trunk_height, z, style.log);
			for (int lx = x - 3; lx <= x + 3; ++lx)
			{
				for (int lz = z - 3; lz <= z + 3; ++lz)
				{
					const int distance = std::abs(lx - x) + std::abs(lz - z);
					if (distance <= 4) SetBlock(world, bounds, lx, y + trunk_height + 1, lz, style.leaves);
					if (distance <= 3) SetBlock(world, bounds, lx, y + trunk_height, lz, style.leaves);
					if (distance <= 2) SetBlock(world, bounds, lx, y + trunk_height + 2, lz, style.leaves);
				}
			}
		}

		void BuildTrees(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.trees || config.tree_count <= 0) return;
			const std::array styles{
				TreeStyle{ BlockId::OakLog, BlockId::OakLeaves, 0 },
				TreeStyle{ BlockId::BirchLog, BlockId::BirchLeaves, 0 },
				TreeStyle{ BlockId::SpruceLog, BlockId::OakLeaves, 2 },
				TreeStyle{ BlockId::CherryLog, BlockId::CherryLeaves, 0 }
			};
			std::mt19937 rng(static_cast<std::uint32_t>(config.seed));
			std::uniform_int_distribution<int> x_dist(10, std::max(11, bounds.width - 11));
			std::uniform_int_distribution<int> z_dist(10, std::max(11, bounds.depth - 11));
			int placed = 0;
			for (int attempt = 0; attempt < config.tree_count * 6 && placed < config.tree_count; ++attempt)
			{
				const int x = x_dist(rng);
				const int z = z_dist(rng);
				if (std::abs(x - bounds.center_x) < 34 && std::abs(z - bounds.center_z) < 30) continue;
				const int y = FindGroundY(world, bounds, x, z, config.ground_y);
				if (world.GetBlock(x, y, z) == BlockId::Water || y < config.ground_y - 4) continue;
				const TreeStyle& style = styles[static_cast<std::size_t>((placed + config.seed) % static_cast<int>(styles.size()))];
				PlaceTree(world, bounds, x, z, y, style);
				++placed;
			}
		}

		void BuildLights(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.lights || config.lantern_count <= 0) return;
			for (int index = 0; index < config.lantern_count; ++index)
			{
				const int side = index % 4;
				const int offset = -24 + (index / 4) * 10;
				const int x = side < 2 ? bounds.center_x + offset : bounds.center_x + (side == 2 ? -29 : 33);
				const int z = side < 2 ? bounds.center_z + (side == 0 ? -5 : 5) : bounds.center_z + offset;
				const int y = FindGroundY(world, bounds, x, z, config.ground_y);
				FillBox(world, bounds, x, y + 1, z, x, y + 3, z, BlockId::OakLog);
				SetBlock(world, bounds, x, y + 4, z, BlockId::SeaLantern);
			}
		}

		void BuildMarket(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.market || config.market_stall_count <= 0) return;
			const int y = config.ground_y;
			const int origin_x = bounds.center_x - 16;
			const int origin_z = bounds.center_z + 9;
			for (int index = 0; index < config.market_stall_count; ++index)
			{
				const int x = origin_x + (index % 4) * 8;
				const int z = origin_z + (index / 4) * 9;
				const BlockId canopy = index % 3 == 0 ? BlockId::HayBlock : (index % 3 == 1 ? BlockId::CherryPlanks : BlockId::BirchPlanks);
				FlattenArea(world, bounds, x - 1, z - 1, x + 5, z + 5, y, BlockId::Gravel);
				FillBox(world, bounds, x, y + 1, z, x, y + 4, z, BlockId::OakLog);
				FillBox(world, bounds, x + 4, y + 1, z, x + 4, y + 4, z, BlockId::OakLog);
				FillBox(world, bounds, x, y + 1, z + 4, x, y + 4, z + 4, BlockId::OakLog);
				FillBox(world, bounds, x + 4, y + 1, z + 4, x + 4, y + 4, z + 4, BlockId::OakLog);
				FillBox(world, bounds, x - 1, y + 5, z - 1, x + 5, y + 5, z + 5, canopy);
				FillBox(world, bounds, x + 1, y + 1, z + 1, x + 3, y + 1, z + 3, index % 2 == 0 ? BlockId::Bookshelf : BlockId::CraftingTable);
				SetBlock(world, bounds, x + 2, y + 2, z + 2, index % 2 == 0 ? BlockId::Pumpkin : BlockId::Melon);
				SetBlock(world, bounds, x + 2, y + 4, z + 2, BlockId::SeaLantern);
			}
		}

		void BuildAncientRuins(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.ruins || config.ruin_count <= 0) return;
			for (int index = 0; index < config.ruin_count; ++index)
			{
				const int x = bounds.center_x + 18 + (index % 4) * 9;
				const int z = bounds.center_z + 37 - (index / 4) * 11;
				const int y = FindGroundY(world, bounds, x, z, config.ground_y);
				const int height = 3 + (index % 4);
				FlattenArea(world, bounds, x - 3, z - 3, x + 3, z + 3, y, BlockId::MossyCobblestone);
				FillBox(world, bounds, x - 3, y + 1, z - 3, x - 3, y + height, z - 3, BlockId::Blackstone);
				FillBox(world, bounds, x + 3, y + 1, z - 3, x + 3, y + height - 1, z - 3, BlockId::Basalt);
				FillBox(world, bounds, x - 3, y + 1, z + 3, x - 3, y + height - 2, z + 3, BlockId::Basalt);
				FillBox(world, bounds, x + 3, y + 1, z + 3, x + 3, y + height, z + 3, BlockId::Blackstone);
				if (index % 2 == 0) FillBox(world, bounds, x - 2, y + height, z, x + 2, y + height, z, BlockId::Blackstone);
				SetBlock(world, bounds, x, y + 1, z, BlockId::AmethystBlock);
				SetBlock(world, bounds, x, y + 2, z, BlockId::SeaLantern);
			}
		}

		void BuildFloatingIslands(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.floating_islands || config.floating_island_count <= 0) return;
			for (int index = 0; index < config.floating_island_count; ++index)
			{
				const int cx = bounds.center_x - 40 + (index % 3) * 35;
				const int cz = bounds.center_z - 50 + (index / 3) * 24;
				const int cy = config.ground_y + 24 + (index % 3) * 4;
				const int radius = 5 + (index % 2);
				for (int dx = -radius; dx <= radius; ++dx)
				{
					for (int dz = -radius; dz <= radius; ++dz)
					{
						for (int dy = -4; dy <= 1; ++dy)
						{
							const float nx = static_cast<float>(dx) / static_cast<float>(radius);
							const float nz = static_cast<float>(dz) / static_cast<float>(radius);
							const float ny = static_cast<float>(dy) / 4.0f;
							if ((nx * nx) + (nz * nz) + (ny * ny) > 1.0f) continue;
							const BlockId block = dy < -2 ? BlockId::Stone : (dy < 0 ? BlockId::Dirt : BlockId::Grass);
							SetBlock(world, bounds, cx + dx, cy + dy, cz + dz, block);
						}
					}
				}
				if (config.water && index % 2 == 0)
				{
					FillBox(world, bounds, cx, config.ground_y + 5, cz, cx, cy, cz, BlockId::Water);
				}
				SetBlock(world, bounds, cx, cy + 2, cz, BlockId::SeaLantern);
				if (config.trees)
				{
					const TreeStyle style{ index % 2 == 0 ? BlockId::CherryLog : BlockId::BirchLog, index % 2 == 0 ? BlockId::CherryLeaves : BlockId::BirchLeaves, 0 };
					PlaceTree(world, bounds, cx + 2, cz + 1, cy + 1, style);
				}
			}
		}

		void BuildBeacon(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.beacon) return;
			const int x = bounds.center_x - 35;
			const int z = bounds.center_z - 34;
			const int y = FindGroundY(world, bounds, x, z, config.ground_y);
			FlattenArea(world, bounds, x - 5, z - 5, x + 5, z + 5, y, BlockId::Blackstone);
			FillBox(world, bounds, x - 3, y + 1, z - 3, x + 3, y + 1, z + 3, BlockId::SeaLantern);
			FillBox(world, bounds, x - 2, y + 2, z - 2, x + 2, y + 2, z + 2, BlockId::Glass);
			FillBox(world, bounds, x, y + 3, z, x, y + 24, z, BlockId::Glass);
			FillBox(world, bounds, x - 1, y + 25, z - 1, x + 1, y + 25, z + 1, BlockId::SeaLantern);
		}

		void BuildBlockShowcase(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int x0 = bounds.center_x - 50;
			const int z0 = bounds.center_z + 48;
			const int y = config.ground_y;
			const std::array blocks{
				BlockId::Grass, BlockId::Dirt, BlockId::Stone, BlockId::Cobblestone, BlockId::Granite,
				BlockId::Andesite, BlockId::Diorite, BlockId::Blackstone, BlockId::Basalt, BlockId::Bricks,
				BlockId::OakPlanks, BlockId::BirchPlanks, BlockId::SprucePlanks, BlockId::CherryPlanks,
				BlockId::SeaLantern, BlockId::Glass, BlockId::Water, BlockId::AmethystBlock
			};
			for (std::size_t index = 0; index < blocks.size(); ++index)
			{
				const int x = x0 + static_cast<int>(index % 9) * 4;
				const int z = z0 + static_cast<int>(index / 9) * 4;
				FillBox(world, bounds, x, y + 1, z, x + 2, y + 3, z + 2, blocks[index]);
			}
			if (!config.stress_blocks) return;
			for (int row = 0; row < 8; ++row)
			{
				for (int column = 0; column < 24; ++column)
				{
					const BlockId block = blocks[static_cast<std::size_t>((row + column) % static_cast<int>(blocks.size()))];
					FillBox(world, bounds, bounds.center_x - 50 + column * 2, y + 1, bounds.center_z - 55 + row * 3,
						bounds.center_x - 49 + column * 2, y + 3 + (column % 4), bounds.center_z - 54 + row * 3, block);
				}
			}
		}

		void BuildMinecraftStyleShowcase(ve::world::World& world, VulkanMinecraftDemoSceneConfig config)
		{
			config = Sanitize(config);
			const DemoBounds bounds = BoundsFor(world);
			BuildTerrain(world, bounds, config);
			BuildLake(world, bounds, config);
			BuildVillage(world, bounds, config);
			BuildFarm(world, bounds, config);
			BuildQuarryAndCave(world, bounds, config);
				BuildWatchtower(world, bounds, config);
				BuildTrees(world, bounds, config);
				BuildLights(world, bounds, config);
				BuildMarket(world, bounds, config);
				BuildAncientRuins(world, bounds, config);
				BuildFloatingIslands(world, bounds, config);
				BuildBeacon(world, bounds, config);
				BuildBlockShowcase(world, bounds, config);
			}
	}

	void VulkanDemoSceneBuilder::Build(ve::world::World& world, ve::rendering::VulkanMinecraftDemoSceneConfig config)
	{
		BuildMinecraftStyleShowcase(world, config);
	}
}
