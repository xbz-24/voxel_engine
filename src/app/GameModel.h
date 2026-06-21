#pragma once

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "BlockSelection.h"
#include "AsyncWorldGenerator.h"
#include "Camera.h"
#include "ChunkMeshPipeline.h"
#include "World.h"

#include <memory>

namespace ve::engine
{
	class GameModel
	{
	public:
		/**
		 * Creates world and player-facing gameplay state.
		 *
		 * @param world_size_chunks Number of chunks along one side of the world.
		 * @param asset_paths Optional resolved asset paths used to load block metadata for legacy gameplay/rendering.
		 */
		explicit GameModel(int world_size_chunks, const ve::assets::AssetPaths* asset_paths = nullptr);

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
		/** @return Mutable block registry used by gameplay and rendering. */
		ve::blocks::BlockRegistry* MutableBlockRegistry() noexcept;
		/** @return Read-only block registry used by gameplay and rendering. */
		const ve::blocks::BlockRegistry* GetBlockRegistry() const noexcept;
		/** Applies completed async terrain chunks to the world. */
		[[nodiscard]] int PumpAsyncWorldGeneration();
		/** @return Number of terrain tasks waiting to start. */
		[[nodiscard]] ve::core::Index PendingWorldGenerationCount() const;
		/** @param block_registry Block metadata. @param render_distance_chunks Chunk radius around the camera. */
		void PumpAsyncChunkMeshing(const ve::blocks::BlockRegistry& block_registry, int render_distance_chunks);

	private:
		Camera camera_;
		ve::world::World world_;
		ve::world::generation::AsyncWorldGenerator world_generator_;
		ve::world::mesh::ChunkMeshPipeline mesh_pipeline_;
		ve::gameplay::BlockSelection block_selection_;
		std::unique_ptr<ve::blocks::BlockRegistry> block_registry_;
	};
}
