#pragma once

#include "Block.h"
#include "WorldBlockEdit.h"

#include <glm/glm.hpp>

#include <vector>

namespace ve::engine
{
	using WorldBlockEdit = ve::world::WorldBlockEdit;

	struct RuntimeInputSnapshot
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
	};

	struct RuntimeCameraState
	{
		glm::vec3 position{ 0.0f };
		glm::vec3 forward{ 0.0f, 0.0f, -1.0f };
	};

	struct RuntimeBlockHitResult
	{
		bool has_hit = false;
		glm::ivec3 target_block{ 0 };
		glm::ivec3 placement_block{ 0 };
		ve::blocks::BlockId target_block_id = ve::blocks::BlockId::Air;
	};

	struct RuntimeFrameContext
	{
		float delta_seconds = 0.0f;
		float elapsed_seconds = 0.0f;
		double fps = 0.0;
		RuntimeInputSnapshot input{};
		RuntimeCameraState camera{};
		ve::blocks::BlockId selected_block = ve::blocks::BlockId::Air;
		RuntimeBlockHitResult hit_result{};
		std::vector<WorldBlockEdit> world_edits;
		bool request_close = false;
	};

	struct RuntimeDiagnostics
	{
		double fps = 0.0;
		int pending_world_events = 0;
		int render_distance_chunks = 0;
		int pending_chunk_mesh_tasks = 0;
		int pending_chunk_mesh_uploads = 0;
		int pending_world_generation_tasks = 0;
	};
}
