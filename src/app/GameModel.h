#pragma once

#include "BlockSelection.h"
#include "AsyncWorldGenerator.h"
#include "Camera.h"
#include "ChunkMeshPipeline.h"
#include "World.h"

namespace ve::engine
{
	class GameModel
	{
	public:
		/**
		 * Creates world and player-facing gameplay state.
		 *
		 * @param world_size_chunks Number of chunks along one side of the world.
		 */
		explicit GameModel(int world_size_chunks);

		/** @return Mutable camera controlled by gameplay input. */
		Camera& MutableCamera() noexcept;
		/** @return Read-only camera used by render systems. */
		const Camera& GetCamera() const noexcept;
		/** @return Mutable voxel world state. */
		ve::world::World& MutableWorld() noexcept;
		/** @return Read-only voxel world state. */
		const ve::world::World& GetWorld() const noexcept;
		/** @return Mutable block selection state. */
		ve::gameplay::BlockSelection& MutableSelection() noexcept;
		/** @return Read-only block selection state. */
		const ve::gameplay::BlockSelection& GetSelection() const noexcept;
		/** Applies completed async terrain chunks to the world. */
		void PumpAsyncWorldGeneration();
		/** @param block_registry Block metadata. @param render_distance_chunks Chunk radius around the camera. */
		void PumpAsyncChunkMeshing(const ve::blocks::BlockRegistry& block_registry, int render_distance_chunks);

	private:
		Camera camera_;
		ve::world::World world_;
		ve::world::generation::AsyncWorldGenerator world_generator_;
		ve::world::mesh::ChunkMeshPipeline mesh_pipeline_;
		ve::gameplay::BlockSelection block_selection_;
	};
}
