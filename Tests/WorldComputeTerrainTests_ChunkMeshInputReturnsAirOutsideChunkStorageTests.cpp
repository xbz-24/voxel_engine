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

TEST_CASE("chunk mesh input returns air outside chunk storage")
{
	std::vector<ve::blocks::BlockId> blocks(ve::world::mesh::ChunkBlockCount, ve::blocks::BlockId::Air);
	blocks.front() = ve::blocks::BlockId::Dirt;
	blocks.back() = ve::blocks::BlockId::Stone;
	const ve::world::mesh::ChunkMeshInput input{ 0, 0, blocks };

	CHECK(input.GetBlock(0, 0, 0) == ve::blocks::BlockId::Dirt);
	CHECK(input.GetBlock(15, 127, 15) == ve::blocks::BlockId::Stone);
	CHECK(input.GetBlock(-1, 0, 0) == ve::blocks::BlockId::Air);
	CHECK(input.GetBlock(16, 0, 0) == ve::blocks::BlockId::Air);
	CHECK(input.GetBlock(0, 128, 0) == ve::blocks::BlockId::Air);
	CHECK(input.GetBlock(0, 0, 16) == ve::blocks::BlockId::Air);
}

namespace
{
	class RecordingComputeDispatcher final : public ve::rendering::ComputeDispatcher
	{
	public:
		explicit RecordingComputeDispatcher(bool supported) noexcept
			: supported_(supported)
		{
		}

		[[nodiscard]] bool IsSupported() const noexcept override
		{
			return supported_;
		}

		void Dispatch(
			ve::rendering::ComputeProgramHandle program,
			ve::rendering::ComputeDispatchSize size) const override
		{
			lastProgram = program;
			lastDispatchSize = size;
			dispatchCount++;
		}

		bool supported_ = false;
		mutable ve::rendering::ComputeProgramHandle lastProgram{};
		mutable ve::rendering::ComputeDispatchSize lastDispatchSize{};
		mutable int dispatchCount = 0;
	};
}

