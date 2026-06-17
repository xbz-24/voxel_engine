#include "GameModel.h"

#include "Logger.h"
#include "VulkanDemoSceneBuilder.h"
#include "WorkerPolicy.h"

#include <string>

namespace ve::engine
{
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

	Camera& GameModel::MutableCamera() noexcept { return camera_; }

	const Camera& GameModel::GetCamera() const noexcept { return camera_; }

	ve::world::World& GameModel::MutableWorld() noexcept { return world_; }

	const ve::world::World& GameModel::GetWorld() const noexcept { return world_; }

	ve::gameplay::BlockSelection& GameModel::MutableSelection() noexcept { return block_selection_; }

	const ve::gameplay::BlockSelection& GameModel::GetSelection() const noexcept { return block_selection_; }

	ve::blocks::BlockRegistry* GameModel::MutableBlockRegistry() noexcept { return block_registry_.get(); }

	const ve::blocks::BlockRegistry* GameModel::GetBlockRegistry() const noexcept { return block_registry_.get(); }

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
			active_vulkan_demo_config_.reset();
		}
	}

	void GameModel::UpdateVulkanDemoScene(const ve::rendering::VulkanMinecraftDemoSceneConfig& scene_config, bool force_rebuild)
	{
		if (world_generator_.PendingTaskCount() != 0) return;
		if (!force_rebuild && active_vulkan_demo_config_ && *active_vulkan_demo_config_ == scene_config) return;
		VulkanDemoSceneBuilder::Build(world_, scene_config);
		active_vulkan_demo_config_ = scene_config;
		VE_LOG_CATEGORY_INFO(ve::log::category::World, "Built editable Vulkan Minecraft demo scene");
	}

	void GameModel::PumpAsyncChunkMeshing(const ve::blocks::BlockRegistry& block_registry, int render_distance_chunks)
	{
		world_.UploadReadyChunkMeshes(mesh_pipeline_);
		world_.ScheduleVisibleChunkMeshes(block_registry, mesh_pipeline_, camera_.GetPosition(), render_distance_chunks);
	}
}
