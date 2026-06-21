#pragma once

#include "RenderApi.h"
#include "Window.h"
#include "Block.h"

#include <glm/glm.hpp>

#include <vector>

namespace ve::engine
{
	struct WorldBlockEdit
	{
		enum class Kind
		{
			SetBlock,
			FillBox
		};

		Kind kind = Kind::SetBlock;
		int min_x = 0;
		int min_y = 0;
		int min_z = 0;
		int max_x = 0;
		int max_y = 0;
		int max_z = 0;
		ve::blocks::BlockId block = ve::blocks::BlockId::Air;
	};

	struct EngineCreateInfo
	{
		WindowCreateInfo window{};
		ve::rendering::RenderBackendConfiguration render_backend{};
		glm::vec3 camera_position{ 80.0f, 50.0f, 80.0f };
		glm::vec3 camera_look_at{ 80.0f, 50.0f, 79.0f };
		bool vsync = false;
		bool show_debug_overlay = true;
		bool has_custom_camera = false;
		int world_size_chunks = 8;
		int render_distance_chunks = 2;
		std::vector<WorldBlockEdit> world_edits;
	};
}
