#pragma once

#include "Block.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace ve::blocks
{
	struct SolidBlockColor
	{
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
	};

	struct SolidBlockPaletteEntry
	{
		BlockId block = BlockId::Air;
		SolidBlockColor color{};
	};

	[[nodiscard]] constexpr SolidBlockColor SolidColor(BlockId block) noexcept
	{
		switch (block)
		{
		case BlockId::Grass: return { 0.26f, 0.46f, 0.20f, 1.0f };
		case BlockId::Dirt: return { 0.45f, 0.31f, 0.20f, 1.0f };
		case BlockId::Stone: return { 0.55f, 0.55f, 0.53f, 1.0f };
		case BlockId::Cobblestone: return { 0.45f, 0.46f, 0.45f, 1.0f };
		case BlockId::OakLog: return { 0.34f, 0.21f, 0.12f, 1.0f };
		case BlockId::OakPlanks: return { 0.50f, 0.33f, 0.18f, 1.0f };
		case BlockId::Sand: return { 0.83f, 0.76f, 0.55f, 1.0f };
		case BlockId::Bricks: return { 0.54f, 0.24f, 0.20f, 1.0f };
		case BlockId::DiamondOre: return { 0.20f, 0.76f, 0.82f, 1.0f };
		case BlockId::CoalOre: return { 0.12f, 0.12f, 0.13f, 1.0f };
		case BlockId::IronOre: return { 0.68f, 0.58f, 0.48f, 1.0f };
		case BlockId::Obsidian: return { 0.09f, 0.07f, 0.14f, 1.0f };
		case BlockId::OakLeaves: return { 0.12f, 0.34f, 0.15f, 1.0f };
		case BlockId::Deepslate: return { 0.25f, 0.26f, 0.27f, 1.0f };
		case BlockId::CopperOre: return { 0.70f, 0.38f, 0.25f, 1.0f };
		case BlockId::GoldOre: return { 0.95f, 0.70f, 0.20f, 1.0f };
		case BlockId::EmeraldOre: return { 0.14f, 0.78f, 0.36f, 1.0f };
		case BlockId::LapisOre: return { 0.18f, 0.28f, 0.82f, 1.0f };
		case BlockId::Gravel: return { 0.49f, 0.48f, 0.46f, 1.0f };
		case BlockId::MossBlock: return { 0.17f, 0.37f, 0.14f, 1.0f };
		case BlockId::Snow: return { 0.88f, 0.89f, 0.86f, 1.0f };
		case BlockId::MossyCobblestone: return { 0.36f, 0.44f, 0.32f, 1.0f };
		case BlockId::Granite: return { 0.61f, 0.43f, 0.37f, 1.0f };
		case BlockId::Andesite: return { 0.55f, 0.56f, 0.55f, 1.0f };
		case BlockId::Diorite: return { 0.77f, 0.76f, 0.72f, 1.0f };
		case BlockId::Blackstone: return { 0.16f, 0.15f, 0.17f, 1.0f };
		case BlockId::Basalt: return { 0.25f, 0.25f, 0.27f, 1.0f };
		case BlockId::Bedrock: return { 0.20f, 0.20f, 0.20f, 1.0f };
		case BlockId::BirchLog: return { 0.61f, 0.56f, 0.44f, 1.0f };
		case BlockId::BirchPlanks: return { 0.62f, 0.49f, 0.27f, 1.0f };
		case BlockId::BirchLeaves: return { 0.30f, 0.50f, 0.20f, 1.0f };
		case BlockId::SpruceLog: return { 0.20f, 0.12f, 0.07f, 1.0f };
		case BlockId::SprucePlanks: return { 0.42f, 0.29f, 0.16f, 1.0f };
		case BlockId::CraftingTable: return { 0.52f, 0.34f, 0.19f, 1.0f };
		case BlockId::Bookshelf: return { 0.56f, 0.38f, 0.21f, 1.0f };
		case BlockId::Pumpkin: return { 0.90f, 0.46f, 0.12f, 1.0f };
		case BlockId::Melon: return { 0.52f, 0.72f, 0.24f, 1.0f };
		case BlockId::HayBlock: return { 0.78f, 0.70f, 0.22f, 1.0f };
		case BlockId::AmethystBlock: return { 0.62f, 0.40f, 0.84f, 1.0f };
		case BlockId::Glass: return { 0.62f, 0.84f, 0.92f, 0.42f };
		case BlockId::Water: return { 0.08f, 0.25f, 0.56f, 0.72f };
		case BlockId::SeaLantern: return { 1.00f, 0.82f, 0.42f, 1.0f };
		case BlockId::CherryLog: return { 0.32f, 0.17f, 0.15f, 1.0f };
		case BlockId::CherryPlanks: return { 0.58f, 0.34f, 0.34f, 1.0f };
		case BlockId::CherryLeaves: return { 0.63f, 0.32f, 0.46f, 1.0f };
		case BlockId::Sandstone: return { 0.75f, 0.67f, 0.46f, 1.0f };
		case BlockId::CutSandstone: return { 0.84f, 0.78f, 0.60f, 1.0f };
		case BlockId::RedSand: return { 0.78f, 0.42f, 0.17f, 1.0f };
		case BlockId::RedSandstone: return { 0.69f, 0.35f, 0.17f, 1.0f };
		case BlockId::Terracotta: return { 0.62f, 0.37f, 0.29f, 1.0f };
		case BlockId::Cactus: return { 0.30f, 0.62f, 0.28f, 1.0f };
		case BlockId::DeadBush: return { 0.42f, 0.34f, 0.20f, 1.0f };
		case BlockId::Air:
		case BlockId::Count:
		default: return { 0.0f, 0.0f, 0.0f, 0.0f };
		}
	}

	[[nodiscard]] inline float SolidColorHash01(int x, int y, int z) noexcept
	{
		std::uint32_t value = static_cast<std::uint32_t>(x) * 0x9E3779B9u;
		value ^= static_cast<std::uint32_t>(y) * 0x85EBCA6Bu;
		value ^= static_cast<std::uint32_t>(z) * 0xC2B2AE35u;
		value ^= value >> 16u;
		value *= 0x7FEB352Du;
		value ^= value >> 15u;
		return static_cast<float>(value & 0xFFFFu) / 65535.0f;
	}

	[[nodiscard]] inline SolidBlockColor JitteredSolidColor(BlockId block, int x, int y, int z) noexcept
	{
		SolidBlockColor color = SolidColor(block);
		float amplitude = 0.10f;
		if (block == BlockId::Grass || block == BlockId::OakLeaves || block == BlockId::BirchLeaves || block == BlockId::MossBlock) amplitude = 0.18f;
		if (block == BlockId::SeaLantern || block == BlockId::Glass || block == BlockId::Water) amplitude = 0.04f;
		const float jitter = (SolidColorHash01(x, y, z) - 0.5f) * amplitude;
		color.r = std::clamp(color.r + jitter, 0.0f, 1.0f);
		color.g = std::clamp(color.g + jitter, 0.0f, 1.0f);
		color.b = std::clamp(color.b + jitter, 0.0f, 1.0f);
		return color;
	}

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
