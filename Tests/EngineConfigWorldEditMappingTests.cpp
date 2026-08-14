#include <doctest/doctest.h>

#include "EngineRuntimeBridge.h"

#include <voxel/EngineConfig.h>

#include <vector>

namespace
{
	void AppendInvalidWorldEdits(std::vector<voxel::WorldEdit>& edits)
	{
		voxel::WorldEdit invalid_kind{};
		invalid_kind.kind = static_cast<voxel::WorldEdit::Kind>(255);
		invalid_kind.block = voxel::Stone;
		edits.push_back(invalid_kind);
		voxel::WorldEdit invalid_block{};
		invalid_block.block = static_cast<voxel::Block>(255);
		edits.push_back(invalid_block);
	}
}

TEST_CASE("runtime callback mapping discards unknown world edit values")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default().OnUpdate([](voxel::FrameContext& frame) {
		AppendInvalidWorldEdits(frame.commands.world_edits);
		frame.commands.SetBlock(7, 8, 9, voxel::DiamondOre);
	});
	const ve::engine::EngineCreateInfo create_info =
		voxel::detail::DefaultEngineConfigTranslator().Translate(config);
	ve::engine::RuntimeFrameContext frame{};
	create_info.on_update(frame);

	REQUIRE(frame.world_edits.size() == 1);
	CHECK(frame.world_edits.front().kind == ve::world::WorldBlockEdit::Kind::SetBlock);
	CHECK(frame.world_edits.front().minimum_block_x == 7);
	CHECK(frame.world_edits.front().minimum_block_y == 8);
	CHECK(frame.world_edits.front().minimum_block_z == 9);
	CHECK(frame.world_edits.front().block == ve::blocks::BlockId::DiamondOre);
}

TEST_CASE("startup mapping does not turn unknown world edits into valid mutations")
{
	voxel::EngineConfig config = voxel::EngineConfig::Default();
	AppendInvalidWorldEdits(config.world.edits);
	config.world.SetBlock(4, 5, 6, voxel::GoldOre);
	const ve::engine::EngineCreateInfo create_info =
		voxel::detail::DefaultEngineConfigTranslator().Translate(config);

	REQUIRE(create_info.world_edits.size() == 1);
	CHECK(create_info.world_edits.front().minimum_block_x == 4);
	CHECK(create_info.world_edits.front().minimum_block_y == 5);
	CHECK(create_info.world_edits.front().minimum_block_z == 6);
	CHECK(create_info.world_edits.front().block == ve::blocks::BlockId::GoldOre);
}
