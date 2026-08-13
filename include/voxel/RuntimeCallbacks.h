#pragma once

#include "voxel/RuntimeFrame.h"

#include <functional>
#include <string>

namespace voxel
{
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
