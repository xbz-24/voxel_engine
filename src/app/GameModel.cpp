#include "GameModel.h"

#include "Logger.h"
#include "WorkerPolicy.h"

#include <string>

namespace ve::engine
{
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

	/// Uploads ready meshes and queues visible chunks that need meshing.
	void GameModel::PumpAsyncChunkMeshing(const ve::blocks::BlockRegistry& block_registry, int render_distance_chunks)
	{
		world_.UploadReadyChunkMeshes(mesh_pipeline_);
		world_.ScheduleVisibleChunkMeshes(block_registry, mesh_pipeline_, camera_.GetPosition(), render_distance_chunks);
	}
}
