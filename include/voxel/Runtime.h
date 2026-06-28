#pragma once

#include "voxel/World.h"

#include <functional>
#include <string>
#include <vector>

namespace voxel
{
	// TODO: Expand this into a key/mouse/gamepad action map instead of hard-coding demo controls.
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
	};

	struct RuntimeCommands
	{
		std::vector<WorldEdit> world_edits;
		bool request_close = false;

		RuntimeCommands& SetBlock(BlockPosition position, Block block);
		RuntimeCommands& SetBlock(int x, int y, int z, Block block);
		RuntimeCommands& FillBox(BlockBox box, Block block);
		RuntimeCommands& FillBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block);
		RuntimeCommands& ClearBox(int x1, int y1, int z1, int x2, int y2, int z2);
		RuntimeCommands& RequestClose() noexcept;
	};

	struct FrameContext
	{
		// TODO: Add accessors for camera, selected block, hit result, and entity ids once runtime systems are public.
		float delta_seconds = 0.0f;
		float elapsed_seconds = 0.0f;
		double fps = 0.0;
		InputSnapshot input{};
		RuntimeCommands commands{};
	};

	struct Diagnostics
	{
		double fps = 0.0;
		int pending_world_events = 0;
		int render_distance_chunks = 0;
	};

	using UpdateCallback = std::function<void(FrameContext&)>;
	using DiagnosticsCallback = std::function<void(const Diagnostics&)>;
	using LogCallback = std::function<void(const std::string&)>;
}
