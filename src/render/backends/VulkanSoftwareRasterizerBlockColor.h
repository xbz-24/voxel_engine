#pragma once

#include "BlockDefinitions.h"
#include "VulkanSoftwareRasterizerColor.h"

namespace ve::rendering
{
	[[nodiscard]] inline Rgb BaseBlockColor(ve::blocks::BlockId block) noexcept
	{
		using enum ve::blocks::BlockId;
		if (block == Grass) return { 92, 159, 74 };
		if (block == Dirt) return { 112, 76, 45 };
		if (block == Stone) return { 124, 124, 124 };
		if (block == Cobblestone) return { 105, 105, 105 };
		if (block == OakLog || block == BirchLog || block == SpruceLog || block == CherryLog) return { 123, 89, 50 };
		if (block == OakPlanks || block == BirchPlanks || block == SprucePlanks || block == CherryPlanks) return { 174, 134, 82 };
		if (block == Sand) return { 214, 198, 129 };
		if (block == Sandstone || block == CutSandstone) return { 196, 170, 103 };
		if (block == RedSand || block == RedSandstone) return { 176, 89, 42 };
		if (block == Terracotta) return { 151, 91, 58 };
		if (block == Cactus) return { 57, 118, 48 };
		if (block == DeadBush) return { 119, 78, 36 };
		if (block == Bricks) return { 151, 74, 60 };
		if (block == DiamondOre) return { 92, 196, 211 };
		if (block == CoalOre) return { 54, 54, 54 };
		if (block == IronOre) return { 216, 170, 130 };
		if (block == Obsidian) return { 42, 32, 65 };
		if (block == OakLeaves || block == BirchLeaves || block == CherryLeaves) return { 67, 137, 58 };
		if (block == Snow) return { 235, 242, 246 };
		if (block == MossBlock) return { 73, 126, 58 };
		if (block == Pumpkin) return { 198, 112, 33 };
		if (block == Melon) return { 104, 158, 53 };
		if (block == HayBlock) return { 203, 174, 63 };
		if (block == AmethystBlock) return { 137, 89, 178 };
		if (block == Glass) return { 148, 214, 219 };
		return { 132, 132, 132 };
	}

	[[nodiscard]] inline Rgb ApplyBlockTint(ve::blocks::BlockId block, ve::blocks::BlockFace face, Rgb color) noexcept
	{
		if (block == ve::blocks::BlockId::Grass && face == ve::blocks::BlockFace::Top)
		{
			return Multiply(color, { 96, 178, 68 }, 0.68f);
		}
		if (block == ve::blocks::BlockId::OakLeaves ||
			block == ve::blocks::BlockId::BirchLeaves ||
			block == ve::blocks::BlockId::CherryLeaves)
		{
			return Multiply(color, { 72, 160, 68 }, 0.58f);
		}
		return color;
	}
}
