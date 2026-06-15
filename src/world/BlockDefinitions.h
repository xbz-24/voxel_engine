#pragma once

#include "Block.h"

#include <cstddef>
#include <string_view>

namespace ve::blocks
{
	struct FaceTextureFiles
	{
		const char* top;
		const char* bottom;
		const char* front;
		const char* back;
		const char* left;
		const char* right;
	};

	struct BlockDefinition
	{
		BlockId id;
		std::string_view name;
		bool isSolid;
		FaceTextureFiles textures;
	};

	/**
	 * Uses one texture for every cube face.
	 */
	constexpr FaceTextureFiles Same(const char* texture)
	{
		return { texture, texture, texture, texture, texture, texture };
	}

	/**
	 * Uses one top, one bottom, and one side texture.
	 */
	constexpr FaceTextureFiles TopBottomSide(const char* top, const char* bottom, const char* side)
	{
		return { top, bottom, side, side, side, side };
	}

	static constexpr BlockDefinition BuiltInBlockDefinitions[] = {
		{ BlockId::Air, "Air", false, Same(nullptr) },
		{ BlockId::Grass, "Grass", true, { "grass_block_top.png", "dirt.png", "grass_block_side.png", "grass_block_side.png", "grass_block_side.png", "grass_block_side.png" } },
		{ BlockId::Dirt, "Dirt", true, Same("dirt.png") },
		{ BlockId::Stone, "Stone", true, Same("stone.png") },
		{ BlockId::Cobblestone, "Cobblestone", true, Same("cobblestone.png") },
		{ BlockId::OakLog, "Oak Log", true, TopBottomSide("oak_log_top.png", "oak_log_top.png", "oak_log.png") },
		{ BlockId::OakPlanks, "Oak Planks", true, Same("oak_planks.png") },
		{ BlockId::Sand, "Sand", true, Same("sand.png") },
		{ BlockId::Bricks, "Bricks", true, Same("bricks.png") },
		{ BlockId::DiamondOre, "Diamond Ore", true, Same("diamond_ore.png") },
		{ BlockId::CoalOre, "Coal Ore", true, Same("coal_ore.png") },
		{ BlockId::IronOre, "Iron Ore", true, Same("iron_ore.png") },
		{ BlockId::Obsidian, "Obsidian", true, Same("obsidian.png") },
		{ BlockId::OakLeaves, "Oak Leaves", true, Same("oak_leaves.png") },
		{ BlockId::Deepslate, "Deepslate", true, Same("deepslate.png") },
		{ BlockId::CopperOre, "Copper Ore", true, Same("copper_ore.png") },
		{ BlockId::GoldOre, "Gold Ore", true, Same("gold_ore.png") },
		{ BlockId::EmeraldOre, "Emerald Ore", true, Same("emerald_ore.png") },
		{ BlockId::LapisOre, "Lapis Ore", true, Same("lapis_ore.png") },
		{ BlockId::Gravel, "Gravel", true, Same("gravel.png") },
		{ BlockId::MossBlock, "Moss", true, Same("moss_block.png") },
		{ BlockId::Snow, "Snow", true, Same("snow.png") },
		{ BlockId::MossyCobblestone, "Mossy Cobble", true, Same("mossy_cobblestone.png") },
		{ BlockId::Granite, "Granite", true, Same("granite.png") },
		{ BlockId::Andesite, "Andesite", true, Same("andesite.png") },
		{ BlockId::Diorite, "Diorite", true, Same("diorite.png") },
		{ BlockId::Blackstone, "Blackstone", true, Same("blackstone.png") },
		{ BlockId::Basalt, "Basalt", true, TopBottomSide("basalt_top.png", "basalt_top.png", "basalt_side.png") },
		{ BlockId::Bedrock, "Bedrock", true, Same("bedrock.png") },
		{ BlockId::BirchLog, "Birch Log", true, TopBottomSide("birch_log_top.png", "birch_log_top.png", "birch_log.png") },
		{ BlockId::BirchPlanks, "Birch Planks", true, Same("birch_planks.png") },
		{ BlockId::BirchLeaves, "Birch Leaves", true, Same("birch_leaves.png") },
		{ BlockId::SpruceLog, "Spruce Log", true, TopBottomSide("spruce_log_top.png", "spruce_log_top.png", "spruce_log.png") },
		{ BlockId::SprucePlanks, "Spruce Planks", true, Same("spruce_planks.png") },
		{ BlockId::CraftingTable, "Crafting Table", true, { "crafting_table_top.png", "oak_planks.png", "crafting_table_front.png", "crafting_table_side.png", "crafting_table_side.png", "crafting_table_side.png" } },
		{ BlockId::Bookshelf, "Bookshelf", true, Same("bookshelf.png") },
		{ BlockId::Pumpkin, "Pumpkin", true, TopBottomSide("pumpkin_top.png", "pumpkin_top.png", "pumpkin_side.png") },
		{ BlockId::Melon, "Melon", true, TopBottomSide("melon_top.png", "melon_top.png", "melon_side.png") },
		{ BlockId::HayBlock, "Hay Bale", true, TopBottomSide("hay_block_top.png", "hay_block_top.png", "hay_block_side.png") },
		{ BlockId::AmethystBlock, "Amethyst", true, Same("amethyst_block.png") },
		{ BlockId::Glass, "Glass", true, Same("glass.png") }
	};

	static_assert((sizeof(BuiltInBlockDefinitions) / sizeof(BuiltInBlockDefinitions[0])) == static_cast<std::size_t>(BlockId::Count), "Every BlockId must have one built-in definition.");
}
