		{ BlockId::CherryLeaves, "Cherry Leaves", true, Same("cherry_leaves.png") },
		{ BlockId::Sandstone, "Sandstone", true, TopBottomSide("sandstone_top.png", "sandstone_bottom.png", "sandstone.png") },
		{ BlockId::CutSandstone, "Cut Sandstone", true, Same("cut_sandstone.png") },
		{ BlockId::RedSand, "Red Sand", true, Same("red_sand.png") },
		{ BlockId::RedSandstone, "Red Sandstone", true, TopBottomSide("red_sandstone_top.png", "red_sandstone_bottom.png", "red_sandstone.png") },
		{ BlockId::Terracotta, "Terracotta", true, Same("terracotta.png") },
		{ BlockId::Cactus, "Cactus", true, TopBottomSide("cactus_top.png", "cactus_bottom.png", "cactus_side.png") },
		{ BlockId::DeadBush, "Dead Bush", true, Same("dead_bush.png") }
	};

	static_assert((sizeof(BuiltInBlockDefinitions) / sizeof(BuiltInBlockDefinitions[0])) == BlockTypeCount, "Every BlockId must have one built-in definition.");
}
