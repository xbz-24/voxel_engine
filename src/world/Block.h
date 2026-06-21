#pragma once

#include <cstdint>

namespace ve::blocks
{
	/**
	 * Identifies the block stored in world/chunk data.
	 */
	enum class BlockId : std::uint8_t
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
		Count
	};

	/**
	 * Names the face of a cube block.
	 */
	enum class BlockFace : std::uint8_t
	{
		Top = 0,
		Bottom,
		Front,
		Back,
		Left,
		Right,
		Count
	};
}
