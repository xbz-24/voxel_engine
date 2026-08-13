#pragma once


#include "voxel/Camera.h"
#include "voxel/WorldTypes.h"

#include <functional>
#include <string>
#include <vector>

namespace voxel
{
	/** Keyboard keys exposed to public frame callbacks. */
	enum class Key
	{
		Unknown,
		W,
		A,
		S,
		D,
		Space,
		Escape,
		F1,
		F2
	};

	/** Semantic input actions mapped from raw runtime input. */
	enum class InputAction
	{
		MoveForward,
		MoveLeft,
		MoveBack,
		MoveRight,
		Jump,
		Cancel,
		ToggleDebugOverlay,
		ToggleRenderMode,
		PrimaryAction
	};

	/** Input state captured for one public update callback frame. */
	struct InputSnapshot
	{
		bool move_forward = false;
		bool move_left = false;
		bool move_back = false;
		bool move_right = false;
		bool jump = false;
		bool escape = false;
		bool f1 = false;
		bool f2 = false;
		bool primary_action = false;
		double mouse_x = 0.0;
		double mouse_y = 0.0;

		/** Returns true when key is down in this frame snapshot. */
		[[nodiscard]] bool IsDown(Key key) const noexcept;

		/** Returns true when action is active in this frame snapshot. */
		[[nodiscard]] bool IsActive(InputAction action) const noexcept;
	};

	/** Camera state exposed to frame callbacks. */
	struct RuntimeCameraState
	{
		Vec3 position{};
		Vec3 forward{};
	};

	/** Block raycast result for the currently selected voxel, if any. */
	struct BlockHitResult
	{
		bool has_hit = false;
		BlockPosition target_block{};
		BlockPosition placement_block{};
		Block target_block_type = Air;
	};

	/** Mutating commands a callback may queue for the runtime to apply after the frame callback returns. */
	struct RuntimeCommands
	{
		std::vector<WorldEdit> world_edits;
		bool request_close = false;

		/** Queues a single block write. */
		RuntimeCommands& SetBlock(BlockPosition position, Block block);

		/** Queues a single block write by coordinates. */
		RuntimeCommands& SetBlock(int block_x, int block_y, int block_z, Block block);

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
