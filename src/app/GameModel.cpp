#include "GameModel.h"

#include "Block.h"
#include "Logger.h"
#include "WorkerPolicy.h"

#include <algorithm>
#include <cstdlib>
#include <string>

namespace ve::engine
{
	namespace
	{
		using ve::blocks::BlockId;

		void FillBox(ve::world::World& world, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z, BlockId block)
		{
			for (int x = min_x; x <= max_x; ++x)
			{
				for (int y = min_y; y <= max_y; ++y)
				{
					for (int z = min_z; z <= max_z; ++z)
					{
						world.SetBlock(x, y, z, block);
					}
				}
			}
		}

		void PlaceTree(ve::world::World& world, int x, int ground_y, int z)
		{
			FillBox(world, x, ground_y + 1, z, x, ground_y + 5, z, BlockId::OakLog);
			for (int lx = x - 2; lx <= x + 2; ++lx)
			{
				for (int lz = z - 2; lz <= z + 2; ++lz)
				{
					const int distance = std::abs(lx - x) + std::abs(lz - z);
					if (distance <= 3) world.SetBlock(lx, ground_y + 6, lz, BlockId::OakLeaves);
					if (distance <= 2) world.SetBlock(lx, ground_y + 5, lz, BlockId::OakLeaves);
				}
			}
			world.SetBlock(x, ground_y + 6, z, BlockId::OakLeaves);
		}

		void BuildHouse(ve::world::World& world, int ground_y)
		{
			FillBox(world, 70, ground_y, 82, 84, ground_y, 96, BlockId::OakPlanks);
			for (int y = ground_y + 1; y <= ground_y + 4; ++y)
			{
				for (int x = 70; x <= 84; ++x)
				{
					world.SetBlock(x, y, 82, BlockId::OakPlanks);
					world.SetBlock(x, y, 96, BlockId::OakPlanks);
				}
				for (int z = 82; z <= 96; ++z)
				{
					world.SetBlock(70, y, z, BlockId::OakPlanks);
					world.SetBlock(84, y, z, BlockId::OakPlanks);
				}
			}

			FillBox(world, 70, ground_y + 1, 82, 70, ground_y + 5, 82, BlockId::OakLog);
			FillBox(world, 84, ground_y + 1, 82, 84, ground_y + 5, 82, BlockId::OakLog);
			FillBox(world, 70, ground_y + 1, 96, 70, ground_y + 5, 96, BlockId::OakLog);
			FillBox(world, 84, ground_y + 1, 96, 84, ground_y + 5, 96, BlockId::OakLog);

			FillBox(world, 69, ground_y + 5, 81, 85, ground_y + 5, 97, BlockId::Cobblestone);
			FillBox(world, 71, ground_y + 6, 83, 83, ground_y + 6, 95, BlockId::Bricks);
			FillBox(world, 73, ground_y + 7, 85, 81, ground_y + 7, 93, BlockId::Bricks);

			FillBox(world, 77, ground_y + 1, 96, 78, ground_y + 3, 96, BlockId::Air);
			world.SetBlock(76, ground_y + 2, 96, BlockId::Glass);
			world.SetBlock(79, ground_y + 2, 96, BlockId::Glass);
			world.SetBlock(70, ground_y + 2, 88, BlockId::Glass);
			world.SetBlock(84, ground_y + 2, 90, BlockId::Glass);

			world.SetBlock(73, ground_y + 1, 85, BlockId::CraftingTable);
			world.SetBlock(74, ground_y + 1, 85, BlockId::Bookshelf);
			world.SetBlock(75, ground_y + 1, 85, BlockId::Bookshelf);
		}

		void BuildTerracedHill(ve::world::World& world, int ground_y)
		{
			for (int x = 88; x <= 105; ++x)
			{
				for (int z = 106; z <= 124; ++z)
				{
					const int distance = std::max(std::abs(x - 97), std::abs(z - 116));
					const int height = distance <= 3 ? 8 : (distance <= 6 ? 5 : (distance <= 9 ? 2 : 0));
					if (height == 0) continue;
					FillBox(world, x, ground_y + 1, z, x, ground_y + height - 1, z, distance <= 4 ? BlockId::Stone : BlockId::Dirt);
					world.SetBlock(x, ground_y + height, z, BlockId::Grass);
					if (height >= 5 && (x + z) % 7 == 0) world.SetBlock(x, ground_y + height + 1, z, BlockId::MossBlock);
				}
			}

			FillBox(world, 94, ground_y + 1, 106, 100, ground_y + 4, 110, BlockId::Air);
			FillBox(world, 93, ground_y + 1, 106, 93, ground_y + 5, 110, BlockId::Blackstone);
			FillBox(world, 101, ground_y + 1, 106, 101, ground_y + 5, 110, BlockId::Blackstone);
			FillBox(world, 94, ground_y + 5, 106, 100, ground_y + 5, 110, BlockId::Basalt);
			world.SetBlock(96, ground_y + 2, 111, BlockId::CoalOre);
			world.SetBlock(98, ground_y + 3, 111, BlockId::IronOre);
			world.SetBlock(100, ground_y + 2, 111, BlockId::CopperOre);
			world.SetBlock(97, ground_y + 1, 110, BlockId::AmethystBlock);
		}

		void BuildGlassPond(ve::world::World& world, int ground_y)
		{
			for (int x = 58; x <= 69; ++x)
			{
				for (int z = 106; z <= 119; ++z)
				{
					const int dx = std::abs(x - 63);
					const int dz = std::abs(z - 112);
					if ((dx * dx) + (dz * dz) <= 45)
					{
						world.SetBlock(x, ground_y, z, BlockId::Glass);
						world.SetBlock(x, ground_y - 1, z, BlockId::Sand);
						if ((dx * dx) + (dz * dz) >= 34) world.SetBlock(x, ground_y + 1, z, BlockId::Sand);
					}
				}
			}
			FillBox(world, 69, ground_y, 111, 78, ground_y, 114, BlockId::Gravel);
		}

		void BuildWatchtower(ve::world::World& world, int ground_y)
		{
			FillBox(world, 102, ground_y + 1, 92, 102, ground_y + 9, 92, BlockId::OakLog);
			FillBox(world, 99, ground_y + 7, 89, 105, ground_y + 7, 95, BlockId::OakPlanks);
			FillBox(world, 99, ground_y + 8, 89, 99, ground_y + 9, 95, BlockId::OakPlanks);
			FillBox(world, 105, ground_y + 8, 89, 105, ground_y + 9, 95, BlockId::OakPlanks);
			FillBox(world, 99, ground_y + 8, 89, 105, ground_y + 9, 89, BlockId::OakPlanks);
			FillBox(world, 99, ground_y + 8, 95, 105, ground_y + 9, 95, BlockId::OakPlanks);
			FillBox(world, 100, ground_y + 10, 90, 104, ground_y + 10, 94, BlockId::Bricks);
			world.SetBlock(102, ground_y + 11, 92, BlockId::AmethystBlock);
			for (int y = ground_y + 1; y <= ground_y + 6; y += 2)
			{
				world.SetBlock(101, y, 92, BlockId::OakPlanks);
				world.SetBlock(103, y + 1, 92, BlockId::OakPlanks);
			}
		}

		void BuildOreGallery(ve::world::World& world, int ground_y)
		{
			FillBox(world, 57, ground_y + 1, 90, 57, ground_y + 5, 101, BlockId::Stone);
			FillBox(world, 58, ground_y + 1, 91, 58, ground_y + 4, 100, BlockId::Air);
			world.SetBlock(57, ground_y + 2, 91, BlockId::CoalOre);
			world.SetBlock(57, ground_y + 3, 93, BlockId::IronOre);
			world.SetBlock(57, ground_y + 2, 95, BlockId::CopperOre);
			world.SetBlock(57, ground_y + 3, 97, BlockId::GoldOre);
			world.SetBlock(57, ground_y + 2, 99, BlockId::DiamondOre);
			FillBox(world, 59, ground_y + 1, 90, 64, ground_y + 1, 90, BlockId::MossyCobblestone);
			FillBox(world, 59, ground_y + 2, 90, 64, ground_y + 2, 90, BlockId::Bookshelf);
		}

		void BuildMinecraftStyleShowcase(ve::world::World& world)
		{
			constexpr int ground_y = 50;
			for (int x = 56; x <= 104; ++x)
			{
				for (int z = 72; z <= 124; ++z)
				{
					FillBox(world, x, ground_y + 1, z, x, ground_y + 11, z, BlockId::Air);
					world.SetBlock(x, ground_y - 3, z, BlockId::Stone);
					world.SetBlock(x, ground_y - 2, z, BlockId::Dirt);
					world.SetBlock(x, ground_y - 1, z, BlockId::Dirt);
					world.SetBlock(x, ground_y, z, BlockId::Grass);
				}
			}

			BuildHouse(world, ground_y);
			BuildTerracedHill(world, ground_y);
			BuildGlassPond(world, ground_y);
			BuildWatchtower(world, ground_y);
			BuildOreGallery(world, ground_y);
			PlaceTree(world, 62, ground_y, 80);
			PlaceTree(world, 96, ground_y, 84);
			PlaceTree(world, 98, ground_y, 116);

			FillBox(world, 78, ground_y, 97, 81, ground_y, 118, BlockId::Gravel);
			FillBox(world, 76, ground_y, 100, 83, ground_y, 103, BlockId::Gravel);
			FillBox(world, 58, ground_y + 1, 112, 63, ground_y + 3, 117, BlockId::HayBlock);
			world.SetBlock(66, ground_y + 1, 104, BlockId::Pumpkin);
			world.SetBlock(67, ground_y + 1, 104, BlockId::Pumpkin);
			world.SetBlock(68, ground_y + 1, 104, BlockId::Melon);
			world.SetBlock(90, ground_y + 1, 104, BlockId::CraftingTable);
			world.SetBlock(92, ground_y + 1, 104, BlockId::Bookshelf);
			world.SetBlock(93, ground_y + 1, 104, BlockId::Bookshelf);
			FillBox(world, 88, ground_y + 1, 114, 92, ground_y + 4, 114, BlockId::MossyCobblestone);
			world.SetBlock(90, ground_y + 5, 114, BlockId::AmethystBlock);
		}
	}

	/// Creates world and player-facing gameplay state.
	GameModel::GameModel(int world_size_chunks, const ve::assets::AssetPaths* asset_paths)
		: world_(ve::world::CreateInfoForSquareWorld(world_size_chunks)),
		  world_generator_(ve::tasks::DefaultWorkerCount()),
		  mesh_pipeline_(ve::tasks::DefaultWorkerCount())
	{
		if (asset_paths != nullptr)
		{
			block_registry_ = std::make_unique<ve::blocks::BlockRegistry>(*asset_paths);
		}
		const ve::world::FlatWorldSpawnSettings settings{ world_size_chunks };
		world_.SpawnEmptyGrid(settings);
		world_generator_.RequestGrid(settings);
	}

	/// Returns the mutable camera controlled by gameplay input.
	Camera& GameModel::MutableCamera() noexcept { return camera_; }

	/// Returns the read-only camera used by render systems.
	const Camera& GameModel::GetCamera() const noexcept { return camera_; }

	/// Returns mutable voxel world state.
	ve::world::World& GameModel::MutableWorld() noexcept { return world_; }

	/// Returns read-only voxel world state.
	const ve::world::World& GameModel::GetWorld() const noexcept { return world_; }

	/// Returns mutable block selection state.
	ve::gameplay::BlockSelection& GameModel::MutableSelection() noexcept { return block_selection_; }

	/// Returns read-only block selection state.
	const ve::gameplay::BlockSelection& GameModel::GetSelection() const noexcept { return block_selection_; }

	/// Returns mutable block registry used by legacy gameplay/rendering.
	ve::blocks::BlockRegistry* GameModel::MutableBlockRegistry() noexcept { return block_registry_.get(); }

	/// Returns read-only block registry used by legacy gameplay/rendering.
	const ve::blocks::BlockRegistry* GameModel::GetBlockRegistry() const noexcept { return block_registry_.get(); }

	/// Applies completed async terrain chunks to the world.
	void GameModel::PumpAsyncWorldGeneration()
	{
		int applied_count = 0;
		for (const ve::world::generation::ChunkGenerationResult& result : world_generator_.DrainCompletedChunks())
		{
			if (world_.ApplyGeneratedChunk(result)) ++applied_count;
		}
		if (applied_count > 0)
		{
			VE_LOG_CATEGORY_DEBUG(ve::log::category::World, std::string("Applied generated chunks: ") + std::to_string(applied_count));
		}
	}

	void GameModel::TryBuildVulkanDemoScene()
	{
		if (vulkan_demo_scene_built_) return;
		if (world_generator_.PendingTaskCount() != 0) return;
		BuildMinecraftStyleShowcase(world_);
		vulkan_demo_scene_built_ = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::World, "Built Vulkan Minecraft-style showcase scene");
	}

	/// Uploads ready meshes and queues visible chunks that need meshing.
	void GameModel::PumpAsyncChunkMeshing(const ve::blocks::BlockRegistry& block_registry, int render_distance_chunks)
	{
		world_.UploadReadyChunkMeshes(mesh_pipeline_);
		world_.ScheduleVisibleChunkMeshes(block_registry, mesh_pipeline_, camera_.GetPosition(), render_distance_chunks);
	}
}
