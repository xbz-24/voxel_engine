		/** Queues a filled-box write. */
		RuntimeCommands& FillBox(BlockBox box, Block block);

		/** Queues a filled-box write between two inclusive corners. */
		RuntimeCommands& FillBox(BlockPosition first, BlockPosition second, Block block);

		/** Queues a filled-box write by inclusive corner coordinates. */
		RuntimeCommands& FillBox(
			int first_block_x,
			int first_block_y,
			int first_block_z,
			int second_block_x,
			int second_block_y,
			int second_block_z,
			Block block);

		/** Queues an air fill between two inclusive corners. */
		RuntimeCommands& ClearBox(BlockPosition first, BlockPosition second);

		/** Queues an air fill by inclusive corner coordinates. */
		RuntimeCommands& ClearBox(
			int first_block_x,
			int first_block_y,
			int first_block_z,
			int second_block_x,
			int second_block_y,
			int second_block_z);

		/** Requests the runtime loop to close at the next safe frame boundary. */
		RuntimeCommands& RequestClose() noexcept;
	};

	/** Per-frame public callback context. */
	struct FrameContext
	{
		float delta_seconds = 0.0f;
		float elapsed_seconds = 0.0f;
		double fps = 0.0;
		InputSnapshot input{};
		RuntimeCameraState camera{};
		Block selected_block = Air;
		BlockHitResult hit_result{};
		// Entity ids stay out of frame callbacks until runtime entity systems are public.
		RuntimeCommands commands{};
	};

	/** Runtime counters surfaced through the diagnostics callback. */
	struct Diagnostics
	{
		double fps = 0.0;
		int pending_world_events = 0;
		int render_distance_chunks = 0;
		int pending_chunk_mesh_tasks = 0;
		int pending_chunk_mesh_uploads = 0;
		int pending_world_generation_tasks = 0;
	};

	using UpdateCallback = std::function<void(FrameContext&)>;
	using DiagnosticsCallback = std::function<void(const Diagnostics&)>;
	using LogCallback = std::function<void(const std::string&)>;
}
