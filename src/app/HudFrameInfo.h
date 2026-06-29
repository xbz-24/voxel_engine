#pragma once

#include "BlockRegistry.h"
#include "Camera.h"
#include "SettingsMenu.h"
#include "Window.h"

#include <cstddef>

#include <glm/glm.hpp>

namespace ve::ui
{
	struct HudFrameInfo
	{
		HudFrameInfo(const ve::engine::Window& frame_window,
			const Camera& frame_camera,
			int fps,
			glm::ivec3 selected_target_block,
			bool has_selected_block,
			const ve::blocks::BlockRegistry& registry,
			ve::blocks::BlockId placement_block,
			bool should_show_debug_overlay,
			bool is_player_flying,
			int render_distance_chunks,
			std::size_t pending_events,
			SettingsMenuState settings_menu_state)
			: window(frame_window),
			  camera(frame_camera),
			  displayed_fps(fps),
			  target_block(selected_target_block),
			  is_block_selected(has_selected_block),
			  block_registry(registry),
			  selected_placement_block(placement_block),
			  show_debug_overlay(should_show_debug_overlay),
			  is_flying(is_player_flying),
			  render_distance_chunks(render_distance_chunks),
			  pending_world_events(pending_events),
			  settings_menu(settings_menu_state)
		{
		}

		const ve::engine::Window& window;
		const Camera& camera;
		int displayed_fps;
		glm::ivec3 target_block;
		bool is_block_selected;
		const ve::blocks::BlockRegistry& block_registry;
		ve::blocks::BlockId selected_placement_block;
		bool show_debug_overlay;
		bool is_flying;
		int render_distance_chunks;
		std::size_t pending_world_events;
		SettingsMenuState settings_menu;
	};
}
