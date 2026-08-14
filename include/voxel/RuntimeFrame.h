#pragma once

#include "voxel/Camera.h"
#include "voxel/RuntimeCommands.h"
#include "voxel/RuntimeInput.h"

namespace voxel
{
	/** @addtogroup voxel_sdk_runtime_callbacks
	 * @{
	 */
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
		RuntimeCommands commands{};
	};
	/** @} */
}
