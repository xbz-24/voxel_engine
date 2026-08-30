#pragma once

#include <cstdint>

namespace voxel
{
	/** Stable block identifiers used by public world-edit and runtime APIs. */
	enum class Block : std::uint8_t
	{
		Air = 0,
		Grass,
		Dirt,
		Stone,
		Cobblestone,
		OakLog,
		OakPlanks,
		Sand,
		Bricks,
		DiamondOre,
		CoalOre,
		IronOre,
		Obsidian,
		OakLeaves,
		Deepslate,
		CopperOre,
		GoldOre,
		EmeraldOre,
		LapisOre,
		Gravel,
		MossBlock,
		Snow,
		MossyCobblestone,
		Granite,
		Andesite,
		Diorite,
		Blackstone,
		Basalt,
		Bedrock,
		BirchLog,
		BirchPlanks,
		BirchLeaves,
		SpruceLog,
		SprucePlanks,
		CraftingTable,
		Bookshelf,
		Pumpkin,
		Melon,
		HayBlock,
		AmethystBlock,
		Glass,
		Water,
		SeaLantern,
		CherryLog,
		CherryPlanks,
		CherryLeaves,
		Sandstone,
		CutSandstone,
		RedSand,
		RedSandstone,
		Terracotta,
		Cactus,
		DeadBush
	};
}
