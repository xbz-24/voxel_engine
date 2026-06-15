#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

struct EngineInputState
{
	bool was_left_mouse_button_pressed = false;
	bool was_right_mouse_button_pressed = false;
	bool was_debug_toggle_pressed = false;
	bool was_fly_toggle_pressed = false;
	bool was_render_distance_decrease_pressed = false;
	bool was_render_distance_increase_pressed = false;
	bool is_grounded = false;
};

struct EngineRenderCacheState
{
	glm::mat4 projection_3d{ 1.0f };
	GLuint cloud_display_list_id = 0;
};

struct EngineWindowState
{
	int current_width = 0;
	int current_height = 0;
};
