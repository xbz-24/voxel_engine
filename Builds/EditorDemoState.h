#pragma once

namespace ve::editor
{
	enum class DemoGame
	{
		MinecraftSandbox,
		VoxelApiShowcase,
		EmptyPrototype
	};

	struct EditorDemoState
	{
		DemoGame selected_demo = DemoGame::MinecraftSandbox;
		int requested_world_size_chunks = 10;
		int render_distance_chunks = 2;
		bool is_vsync_enabled = false;
		bool show_imgui_demo_window = false;
		bool request_demo_rebuild = false;
	};

	/** @param demo Demo id to name. @return Stable UI label for the demo. */
	const char* DemoGameName(DemoGame demo) noexcept;
}
