		WorldConfig& AddPathX(int start_x, int end_x, int block_y, int center_z, int half_width = 1, Block block = Gravel);

		/** Adds a z-axis path strip. */
		WorldConfig& AddPathZ(int center_x, int block_y, int start_z, int end_z, int half_width = 1, Block block = Gravel);

		/** Adds an x-axis bridge with rail blocks. */
		WorldConfig& AddBridgeX(int start_x, int end_x, int block_y, int center_z, int half_width = 1, Block deck = OakPlanks, Block rail = OakLog);

		/** Adds a z-axis bridge with rail blocks. */
		WorldConfig& AddBridgeZ(int center_x, int block_y, int start_z, int end_z, int half_width = 1, Block deck = OakPlanks, Block rail = OakLog);

		/** Adds a small house helper. */
		WorldConfig& AddHouse(int center_x, int base_y, int center_z, int radius = 3, Block walls = OakPlanks, Block roof = Bricks);

		/** Adds a pond helper with rim blocks. */
		WorldConfig& AddPond(int center_x, int block_y, int center_z, int radius = 3, Block water = Water, Block rim = MossyCobblestone);

		/** Adds a garden helper. */
		WorldConfig& AddGarden(int center_x, int block_y, int center_z, int half_width = 2, int half_depth = 3, Block soil = Dirt, Block crop = HayBlock);

		/** Adds a simple tree helper. */
		WorldConfig& AddTree(int trunk_x, int base_y, int trunk_z, Block trunk = OakLog, Block leaves = OakLeaves);
	};

	/** Creates a square world config by chunk count. */
	[[nodiscard]] WorldConfig World(int size_chunks);

	/** Creates a square world config by strongly named chunk count. */
	[[nodiscard]] WorldConfig World(ChunkCount size_chunks);

	/** Creates a default world config. */
	[[nodiscard]] WorldConfig World();

	/** Alias for World(int) when the config is authored as scene content. */
	[[nodiscard]] WorldConfig Scene(int size_chunks);

	/** Alias for World(ChunkCount) when the config is authored as scene content. */
	[[nodiscard]] WorldConfig Scene(ChunkCount size_chunks);

	/** Alias for World() when the config is authored as scene content. */
	[[nodiscard]] WorldConfig Scene();

	/** Writes a public world config to disk. */
	[[nodiscard]] bool SaveWorldConfig(const WorldConfig& world, const std::string& path);

	/** Reads a public world config from disk, returning a default config when loading fails. */
	[[nodiscard]] WorldConfig LoadWorldConfig(const std::string& path);
}
