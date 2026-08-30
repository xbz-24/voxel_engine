#pragma once

#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <limits>

namespace public_world_geometry_tests
{
	inline constexpr int kIntMinimum = (std::numeric_limits<int>::min)();
	inline constexpr int kIntMaximum = (std::numeric_limits<int>::max)();

	[[nodiscard]] inline bool SamePosition(
		const voxel::BlockPosition& first, const voxel::BlockPosition& second) noexcept
	{
		return first.x == second.x && first.y == second.y && first.z == second.z;
	}

	[[nodiscard]] inline bool SameEdit(
		const voxel::WorldEdit& first, const voxel::WorldEdit& second) noexcept
	{
		return first.kind == second.kind && SamePosition(first.position, second.position)
			&& SamePosition(first.box.minimum, second.box.minimum)
			&& SamePosition(first.box.maximum, second.box.maximum)
			&& first.block == second.block;
	}

	[[nodiscard]] inline voxel::WorldConfig SentinelWorld()
	{
		return voxel::World().SetBlock(17, 23, -31, voxel::DiamondOre);
	}

	inline void CheckOnlySentinel(const voxel::WorldConfig& world)
	{
		const voxel::WorldConfig expected = SentinelWorld();
		REQUIRE(world.edits.size() == 1U);
		CHECK(SameEdit(world.edits.front(), expected.edits.front()));
	}

	template <typename Operation>
	void CheckComposedNoOp(Operation operation)
	{
		auto world = SentinelWorld();
		operation(world);
		CheckOnlySentinel(world);
	}

	inline void CheckSetEdit(const voxel::WorldEdit& edit,
		voxel::BlockPosition position, voxel::Block block)
	{
		CHECK(edit.kind == voxel::WorldEdit::Kind::SetBlock);
		CHECK(SamePosition(edit.position, position));
		CHECK(SamePosition(edit.box.minimum, voxel::BlockPosition{}));
		CHECK(SamePosition(edit.box.maximum, voxel::BlockPosition{}));
		CHECK(edit.block == block);
	}

	inline void CheckFillEdit(const voxel::WorldEdit& edit, voxel::BlockPosition minimum,
		voxel::BlockPosition maximum, voxel::Block block)
	{
		CHECK(edit.kind == voxel::WorldEdit::Kind::FillBox);
		CHECK(SamePosition(edit.position, voxel::BlockPosition{}));
		CHECK(SamePosition(edit.box.minimum, minimum));
		CHECK(SamePosition(edit.box.maximum, maximum));
		CHECK(edit.block == block);
	}
}
