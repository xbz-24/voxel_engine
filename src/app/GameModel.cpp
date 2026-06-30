#include "GameModel.h"

#include "Logger.h"
#include "WorkerPolicy.h"

#include <string>

namespace ve::engine
{
	GameModel::GameModel(int worldSizeChunks,
		const ve::assets::AssetPaths* assetPaths,
		ve::blocks::BlockRegistry::TextureLoading textureLoading,
		const ve::world::TerrainGenerationSettings& terrainGeneration)
		: world_(ve::world::CreateInfoForSquareWorld(worldSizeChunks)),
		  world_generator_(ve::tasks::DefaultWorkerCount()),
		  mesh_pipeline_(ve::tasks::DefaultWorkerCount())
	{
		if (assetPaths != nullptr)
		{
			block_registry_ = std::make_unique<ve::blocks::BlockRegistry>(*assetPaths, textureLoading);
		}
		const ve::world::FlatWorldSpawnSettings settings{ worldSizeChunks, terrainGeneration };
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

	int GameModel::PumpAsyncWorldGeneration()
	{
		int appliedChunkCount = 0;
		for (const ve::world::generation::ChunkGenerationResult& result : world_generator_.DrainCompletedChunks())
		{
			if (world_.ApplyGeneratedChunk(result)) ++appliedChunkCount;
		}
		if (appliedChunkCount > 0)
		{
			VE_LOG_CATEGORY_DEBUG(
				ve::log::category::World,
				std::string("Applied generated chunks: ") + std::to_string(appliedChunkCount));
		}
		return appliedChunkCount;
	}

	ve::world::WorldMetrics GameModel::GetWorldMetrics() const
	{
		ve::world::WorldMetrics worldMetrics = world_.Metrics();
		const ve::world::mesh::ChunkMeshPipelineStats meshPipelineStats = mesh_pipeline_.Stats();
		worldMetrics.pendingChunkMeshTaskCount = meshPipelineStats.pendingBuildTaskCount;
		worldMetrics.pendingChunkMeshUploadCount = meshPipelineStats.pendingUploadCount;
		worldMetrics.pendingWorldGenerationTaskCount = world_generator_.PendingTaskCount();
		return worldMetrics;
	}

	ve::core::Index GameModel::PendingWorldGenerationCount() const
	{
		return world_generator_.PendingTaskCount();
	}

	void GameModel::PumpAsyncChunkMeshing(const ve::blocks::BlockRegistry& blockRegistry, int renderDistanceChunks)
	{
		world_.UploadReadyChunkMeshes(mesh_pipeline_);
		world_.ScheduleVisibleChunkMeshes(blockRegistry, mesh_pipeline_, camera_.GetPosition(), renderDistanceChunks);
	}
}
