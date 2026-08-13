#include "VoxelSandboxModule.h"

#include "Engine.h"
#include "RuntimeRenderDriver.h"

namespace ve::engine
{
	VoxelSandboxModule::VoxelSandboxModule(EngineApplication& engine) noexcept
		: engine_(engine)
	{
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		runtime_settings_.renderer.is_vsync_enabled = create_info.vsync;
		runtime_settings_.renderer.show_debug_overlay = create_info.show_debug_overlay;
		runtime_settings_.renderer.render_distance_chunks =
			ve::gameplay::ClampRenderDistanceChunks(create_info.render_distance_chunks);
		runtime_settings_.renderer.backend_configuration = create_info.render_backend;
		runtime_settings_.editor.settings_menu_enabled = create_info.settings_menu_enabled;
	}

	VoxelSandboxModule::~VoxelSandboxModule() = default;
}
