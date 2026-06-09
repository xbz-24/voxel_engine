#include "GameModel.h"

#include "WorkerPolicy.h"

namespace ve::engine
{
	/// Creates world and player-facing gameplay state.
	GameModel::GameModel(int world_size_chunks)
		: world_(ve::world::CreateInfoForSquareWorld(world_size_chunks)),
		  world_generator_(ve::tasks::DefaultWorkerCount()),
		  mesh_pipeline_(ve::tasks::DefaultWorkerCount())
	{
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

	/// Applies completed async terrain chunks to the world.
	void GameModel::PumpAsyncWorldGeneration()
	{
		for (const ve::world::generation::ChunkGenerationResult& result : world_generator_.DrainCompletedChunks())
		{
			world_.ApplyGeneratedChunk(result);
		}
	}

	/// Uploads ready meshes and queues visible chunks that need meshing.
	void GameModel::PumpAsyncChunkMeshing(const ve::blocks::BlockRegistry& block_registry, int render_distance_chunks)
	{
		world_.UploadReadyChunkMeshes(mesh_pipeline_);
		world_.ScheduleVisibleChunkMeshes(block_registry, mesh_pipeline_, camera_.GetPosition(), render_distance_chunks);
	}
}
