#pragma once

#include "BlockSolidColorPalette.h"

#include <array>

namespace ve::blocks
{
	[[nodiscard]] inline BlockId NearestSolidColorBlock(SolidBlockColor color) noexcept
	{
		static constexpr std::array<SolidBlockPaletteEntry, 31> palette{ {
			{ BlockId::Snow, SolidColor(BlockId::Snow) },
			{ BlockId::Sand, SolidColor(BlockId::Sand) },
			{ BlockId::Sandstone, SolidColor(BlockId::Sandstone) },
			{ BlockId::CutSandstone, SolidColor(BlockId::CutSandstone) },
			{ BlockId::RedSand, SolidColor(BlockId::RedSand) },
			{ BlockId::RedSandstone, SolidColor(BlockId::RedSandstone) },
			{ BlockId::Terracotta, SolidColor(BlockId::Terracotta) },
			{ BlockId::Stone, SolidColor(BlockId::Stone) },
			{ BlockId::Gravel, SolidColor(BlockId::Gravel) },
			{ BlockId::Granite, SolidColor(BlockId::Granite) },
			{ BlockId::Andesite, SolidColor(BlockId::Andesite) },
			{ BlockId::Diorite, SolidColor(BlockId::Diorite) },
			{ BlockId::Blackstone, SolidColor(BlockId::Blackstone) },
			{ BlockId::OakLog, SolidColor(BlockId::OakLog) },
			{ BlockId::OakPlanks, SolidColor(BlockId::OakPlanks) },
			{ BlockId::BirchPlanks, SolidColor(BlockId::BirchPlanks) },
			{ BlockId::Cactus, SolidColor(BlockId::Cactus) },
			{ BlockId::MossBlock, SolidColor(BlockId::MossBlock) },
			{ BlockId::OakLeaves, SolidColor(BlockId::OakLeaves) },
			{ BlockId::BirchLeaves, SolidColor(BlockId::BirchLeaves) },
			{ BlockId::Water, SolidColor(BlockId::Water) },
			{ BlockId::Glass, SolidColor(BlockId::Glass) },
			{ BlockId::SeaLantern, SolidColor(BlockId::SeaLantern) },
			{ BlockId::DiamondOre, SolidColor(BlockId::DiamondOre) },
			{ BlockId::LapisOre, SolidColor(BlockId::LapisOre) },
			{ BlockId::AmethystBlock, SolidColor(BlockId::AmethystBlock) },
			{ BlockId::CherryPlanks, SolidColor(BlockId::CherryPlanks) },
			{ BlockId::CherryLeaves, SolidColor(BlockId::CherryLeaves) },
			{ BlockId::GoldOre, SolidColor(BlockId::GoldOre) },
			{ BlockId::IronOre, SolidColor(BlockId::IronOre) },
			{ BlockId::CoalOre, SolidColor(BlockId::CoalOre) }
		} };

		BlockId nearest = BlockId::Snow;
		float best_distance = 100.0f;
		for (const SolidBlockPaletteEntry& entry : palette)
		{
			const float dr = color.r - entry.color.r;
			const float dg = color.g - entry.color.g;
			const float db = color.b - entry.color.b;
			const float distance = (dr * dr) + (dg * dg) + (db * db);
			if (distance < best_distance)
			{
				best_distance = distance;
				nearest = entry.block;
			}
		}
		return nearest;
	}
}
