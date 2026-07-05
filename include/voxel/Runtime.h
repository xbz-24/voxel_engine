#pragma once

#include "voxel/Camera.h"
#include "voxel/World.h"

#include <functional>
#include <string>
#include <vector>

namespace voxel
{
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

		[[nodiscard]] bool IsDown(Key key) const noexcept;
		[[nodiscard]] bool IsActive(InputAction action) const noexcept;
	};

	struct RuntimeCameraState
	{
		Vec3 position{};
		Vec3 forward{};
	};

	struct BlockHitResult
	{
		bool has_hit = false;
		BlockPosition target_block{};
		BlockPosition placement_block{};
		Block target_block_type = Air;
	};

	struct RuntimeCommands
	{
		std::vector<WorldEdit> world_edits;
		bool request_close = false;

		RuntimeCommands& SetBlock(BlockPosition position, Block block);
		RuntimeCommands& SetBlock(int block_x, int block_y, int block_z, Block block);
		RuntimeCommands& FillBox(BlockBox box, Block block);
		RuntimeCommands& FillBox(BlockPosition first, BlockPosition second, Block block);
		RuntimeCommands& FillBox(
			int first_block_x,
			int first_block_y,
			int first_block_z,
			int second_block_x,
			int second_block_y,
			int second_block_z,
			Block block);
		RuntimeCommands& ClearBox(BlockPosition first, BlockPosition second);
		RuntimeCommands& ClearBox(
			int first_block_x,
			int first_block_y,
			int first_block_z,
			int second_block_x,
			int second_block_y,
			int second_block_z);
		RuntimeCommands& RequestClose() noexcept;
	};

	struct FrameContext
	{
		float delta_seconds = 0.0f;
		float elapsed_seconds = 0.0f;
		double fps = 0.0;
		InputSnapshot input{};
		RuntimeCameraState camera{};
		Block selected_block = Air;
		BlockHitResult hit_result{};
		// TODO: Add stable entity ids once runtime entity systems are public.
		RuntimeCommands commands{};
	};

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
