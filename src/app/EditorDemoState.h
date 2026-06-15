#pragma once

namespace ve::editor
{
	/**
	 * @brief Defines the available demonstrative environments.
	 */

	enum class DemoGame
	{
		MinecraftSandbox, 
		VoxelApiShowcase,
		EmptyPrototype
	};

	/**
	 * @brief Encapsulates the configuration and execution state of the editor.
	 *
	 * Acts as the centralized data payload bridging the Immediate Mode GUI and the
	 * core engine logic. All members are inline-initialized to guarantee a
	 * deterministic baseline state upon instantiation.
	 */

	struct EditorDemoState
	{
		DemoGame selected_demo = DemoGame::MinecraftSandbox;
		int requested_world_size_chunks = 10;
		int render_distance_chunks = 2;
		bool is_vsync_enabled = false;
		bool show_imgui_demo_window = false;
		bool request_demo_rebuild = false;
	};

	/**
	 * @brief Retrieves the human-readable UI label for a given demo state.
	 *
	 * @param demo The demo game identifier to evaluate.
	 * @return A null-terminated string literal representing the demo's name.
	 */

	const char* DemoGameName(DemoGame demo) noexcept;
}
