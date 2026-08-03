#pragma once

#include "RenderDistanceSettings.h"

namespace ve::editor
{
	/** Mutable developer settings shared by the OpenGL tools panel and runtime. */
	struct EditorSettings
	{
		int render_distance_chunks = ve::gameplay::DefaultRenderDistanceChunks;
		bool is_vsync_enabled = false;
	};
}
