#include "GameModel.h"

namespace ve::engine
{
	/// Creates world and player-facing gameplay state.
	GameModel::GameModel(int world_size_chunks)
		: world_(ve::world::CreateInfoForSquareWorld(world_size_chunks))
	{
		world_.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ world_size_chunks });
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
}
