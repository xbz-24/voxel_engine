#include <doctest/doctest.h>

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "ChunkGpuMesh.h"
#include "ChunkMeshBuilder.h"
#include "ChunkMeshInput.h"
#include "ComputeTerrainGenerator.h"
#include "GreedyChunkMesher.h"
#include "RenderBackend.h"
#include "World.h"
#include "WorldBlockEdit.h"
#include "WorldCoordinates.h"
#include "TestTypeHelpers.h"

#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>

TEST_CASE("world event contract includes lifecycle tooling events")
{
	const ve::world::WorldEvent chunk_loaded{ ve::world::ChunkLoadedEvent{ 2, 3 } };
	const ve::world::WorldEvent chunk_unloaded{ ve::world::ChunkUnloadedEvent{ 4, 5 } };
	const ve::world::WorldEvent entity_changed{ ve::world::EntityChangedEvent{ 42U, "transform" } };
	const ve::world::WorldEvent region_saved{ ve::world::RegionSavedEvent{ -1, -2, 3, 4, "region-0.voxels" } };

	CHECK(chunk_loaded.Type() == ve::world::WorldEventType::ChunkLoaded);
	REQUIRE(chunk_loaded.AsChunkLoaded() != nullptr);
	CHECK(chunk_loaded.AsChunkLoaded()->chunkCoordinateX == 2);
	CHECK(chunk_unloaded.Type() == ve::world::WorldEventType::ChunkUnloaded);
	REQUIRE(chunk_unloaded.AsChunkUnloaded() != nullptr);
	CHECK(chunk_unloaded.AsChunkUnloaded()->chunkCoordinateZ == 5);
	CHECK(entity_changed.Type() == ve::world::WorldEventType::EntityChanged);
	REQUIRE(entity_changed.AsEntityChanged() != nullptr);
	CHECK(entity_changed.AsEntityChanged()->entityId == 42U);
	CHECK(entity_changed.AsEntityChanged()->reason == "transform");
	CHECK(region_saved.Type() == ve::world::WorldEventType::RegionSaved);
	REQUIRE(region_saved.AsRegionSaved() != nullptr);
	CHECK(region_saved.AsRegionSaved()->maximumChunkCoordinateZ == 4);
	CHECK(region_saved.AsRegionSaved()->destination == "region-0.voxels");

	const ve::world::WorldEventFilter lifecycle_filter = ve::world::WorldEventFilter::ChunkLifecycleOnly();
	CHECK(lifecycle_filter.Includes(ve::world::WorldEventType::ChunkLoaded));
	CHECK(lifecycle_filter.Includes(ve::world::WorldEventType::ChunkUnloaded));
	CHECK(!lifecycle_filter.Includes(ve::world::WorldEventType::BlockChanged));
	CHECK(ve::world::WorldEventFilter::EntityChangesOnly().Includes(ve::world::WorldEventType::EntityChanged));
	CHECK(ve::world::WorldEventFilter::RegionPersistenceOnly().Includes(ve::world::WorldEventType::RegionSaved));
}

