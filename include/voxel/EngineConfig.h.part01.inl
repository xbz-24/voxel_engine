
#include "voxel/Assets.h"
#include "voxel/Camera.h"
#include "voxel/EngineConfigTypes.h"
#include "voxel/Materials.h"
#include "voxel/Runtime.h"
#include "voxel/Scene.h"
#include "voxel/WorldConfig.h"

#include <string>
#include <vector>

namespace voxel
{
	/** Startup-only settings extracted from EngineConfig before runtime systems are created. */
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

	/** Per-frame runtime tuning extracted from EngineConfig after startup validation. */
	struct EngineRuntimeTuning
	{
		int render_distance_chunks = 8;
		bool show_debug_overlay = true;
		bool enable_settings_menu = true;
	};

	/** Main fluent configuration object for launching or embedding the engine. */
	struct EngineConfig
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
		int render_distance_chunks = 8;
		bool show_debug_overlay = true;
		bool enable_settings_menu = true;
		UpdateCallback on_update{};
		DiagnosticsCallback on_diagnostics{};
		LogCallback on_log{};

		/** Returns the default runtime configuration. */
		[[nodiscard]] static EngineConfig Default();
