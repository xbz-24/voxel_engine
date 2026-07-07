namespace voxel::detail
{
	namespace
	{
		[[nodiscard]] ve::blocks::BlockId ToInternalBlock(Block block) noexcept
		{
			switch (block)
			{
			case Block::Air: return ve::blocks::BlockId::Air;
			case Block::Grass: return ve::blocks::BlockId::Grass;
			case Block::Dirt: return ve::blocks::BlockId::Dirt;
			case Block::Stone: return ve::blocks::BlockId::Stone;
			case Block::Cobblestone: return ve::blocks::BlockId::Cobblestone;
			case Block::OakLog: return ve::blocks::BlockId::OakLog;
			case Block::OakPlanks: return ve::blocks::BlockId::OakPlanks;
			case Block::Sand: return ve::blocks::BlockId::Sand;
			case Block::Bricks: return ve::blocks::BlockId::Bricks;
			case Block::DiamondOre: return ve::blocks::BlockId::DiamondOre;
			case Block::CoalOre: return ve::blocks::BlockId::CoalOre;
			case Block::IronOre: return ve::blocks::BlockId::IronOre;
			case Block::Obsidian: return ve::blocks::BlockId::Obsidian;
			case Block::OakLeaves: return ve::blocks::BlockId::OakLeaves;
			case Block::Deepslate: return ve::blocks::BlockId::Deepslate;
			case Block::CopperOre: return ve::blocks::BlockId::CopperOre;
			case Block::GoldOre: return ve::blocks::BlockId::GoldOre;
			case Block::EmeraldOre: return ve::blocks::BlockId::EmeraldOre;
			case Block::LapisOre: return ve::blocks::BlockId::LapisOre;
			case Block::Gravel: return ve::blocks::BlockId::Gravel;
			case Block::MossBlock: return ve::blocks::BlockId::MossBlock;
			case Block::Snow: return ve::blocks::BlockId::Snow;
			case Block::MossyCobblestone: return ve::blocks::BlockId::MossyCobblestone;
			case Block::Granite: return ve::blocks::BlockId::Granite;
			case Block::Andesite: return ve::blocks::BlockId::Andesite;
			case Block::Diorite: return ve::blocks::BlockId::Diorite;
			case Block::Blackstone: return ve::blocks::BlockId::Blackstone;
			case Block::Basalt: return ve::blocks::BlockId::Basalt;
			case Block::Bedrock: return ve::blocks::BlockId::Bedrock;
			case Block::BirchLog: return ve::blocks::BlockId::BirchLog;
			case Block::BirchPlanks: return ve::blocks::BlockId::BirchPlanks;
			case Block::BirchLeaves: return ve::blocks::BlockId::BirchLeaves;
			case Block::SpruceLog: return ve::blocks::BlockId::SpruceLog;
			case Block::SprucePlanks: return ve::blocks::BlockId::SprucePlanks;
			case Block::CraftingTable: return ve::blocks::BlockId::CraftingTable;
			case Block::Bookshelf: return ve::blocks::BlockId::Bookshelf;
			case Block::Pumpkin: return ve::blocks::BlockId::Pumpkin;
			case Block::Melon: return ve::blocks::BlockId::Melon;
			case Block::HayBlock: return ve::blocks::BlockId::HayBlock;
			case Block::AmethystBlock: return ve::blocks::BlockId::AmethystBlock;
			case Block::Glass: return ve::blocks::BlockId::Glass;
			case Block::Water: return ve::blocks::BlockId::Water;
			case Block::SeaLantern: return ve::blocks::BlockId::SeaLantern;
			case Block::CherryLog: return ve::blocks::BlockId::CherryLog;
			case Block::CherryPlanks: return ve::blocks::BlockId::CherryPlanks;
			case Block::CherryLeaves: return ve::blocks::BlockId::CherryLeaves;
			case Block::Sandstone: return ve::blocks::BlockId::Sandstone;
			case Block::CutSandstone: return ve::blocks::BlockId::CutSandstone;
			case Block::RedSand: return ve::blocks::BlockId::RedSand;
			case Block::RedSandstone: return ve::blocks::BlockId::RedSandstone;
			case Block::Terracotta: return ve::blocks::BlockId::Terracotta;
			case Block::Cactus: return ve::blocks::BlockId::Cactus;
			case Block::DeadBush: return ve::blocks::BlockId::DeadBush;
			default: return ve::blocks::BlockId::Air;
			}
		}

		[[nodiscard]] Block ToPublicBlock(ve::blocks::BlockId block) noexcept
		{
			const auto value = static_cast<std::uint8_t>(block);
			const auto last_public_value = static_cast<std::uint8_t>(Block::DeadBush);
			if (value > last_public_value)
			{
				return Block::Air;
			}
			return static_cast<Block>(value);
		}
