#pragma once

#include "voxel/Assets.h"
#include "voxel/Camera.h"
#include "voxel/EngineConfigTypes.h"
#include "voxel/Materials.h"
#include "voxel/Scene.h"
#include "voxel/WorldConfig.h"

namespace voxel
{
	/** Startup-only settings extracted before runtime systems are created. */
	struct EngineStartupConfig
	{
		WindowConfig window{};
		WorldConfig world{};
		AssetCatalog assets{};
		MaterialLibrary materials{};
		SceneGraph scene_graph{};
		CameraConfig camera{};
		LogSettings logging{};
		GraphicsApi graphics_api = GraphicsApi::Vulkan;
		VoxelRenderStyle voxel_render_style{};
	};

	/** Per-frame runtime tuning extracted after startup validation. */
	struct EngineRuntimeTuning
	{
		int render_distance_chunks = 8;
		bool show_debug_overlay = true;
		bool enable_settings_menu = true;
	};
}
