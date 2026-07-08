#pragma once

#include <array>
#include <cstddef>
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
		Sandstone,
		CutSandstone,
		RedSand,
		RedSandstone,
		Terracotta,
		Cactus,
		DeadBush,
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

	using BlockOrdinal = std::size_t;
	using BlockFaceOrdinal = std::size_t;

	inline constexpr BlockOrdinal BlockTypeCount = static_cast<BlockOrdinal>(BlockId::Count);
	inline constexpr BlockFaceOrdinal BlockFaceCount = static_cast<BlockFaceOrdinal>(BlockFace::Count);
	inline constexpr std::size_t BlockFaceTextureSlotCount = BlockTypeCount * BlockFaceCount;
	inline constexpr std::array<BlockFace, BlockFaceCount> AllBlockFaces{ {
		BlockFace::Top,
		BlockFace::Bottom,
		BlockFace::Front,
		BlockFace::Back,
		BlockFace::Left,
		BlockFace::Right
	} };

	template <typename Value>
	using PerBlockFaceArray = std::array<Value, BlockFaceCount>;

	[[nodiscard]] constexpr BlockOrdinal ToBlockOrdinal(BlockId id) noexcept
	{
		return static_cast<BlockOrdinal>(id);
	}

	[[nodiscard]] constexpr BlockFaceOrdinal ToBlockFaceOrdinal(BlockFace face) noexcept
	{
		return static_cast<BlockFaceOrdinal>(face);
	}

	[[nodiscard]] constexpr std::uint8_t ToBlockByte(BlockId id) noexcept
	{
		return static_cast<std::uint8_t>(id);
	}

	[[nodiscard]] constexpr BlockId BlockIdFromByte(std::uint8_t id) noexcept
	{
		return static_cast<BlockId>(id);
	}

	[[nodiscard]] constexpr bool IsStoredBlockId(BlockId id) noexcept
	{
		return ToBlockOrdinal(id) < BlockTypeCount;
	}

	[[nodiscard]] constexpr bool IsStoredBlockByte(std::uint8_t id) noexcept
	{
		return id < ToBlockByte(BlockId::Count);
	}

	[[nodiscard]] constexpr std::size_t BlockFaceTextureSlot(BlockId block, BlockFace face) noexcept
	{
		return (ToBlockOrdinal(block) * BlockFaceCount) + ToBlockFaceOrdinal(face);
	}
}
