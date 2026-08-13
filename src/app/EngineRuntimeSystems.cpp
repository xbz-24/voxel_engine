#include "EngineRuntime.h"

#include "RuntimeRenderFrame.h"

#include <glm/glm.hpp>

#include <memory>

namespace ve::engine
{
	EngineStartupResult EngineRuntime::CreateRuntimeSystems()
	{
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		const EngineStartupResult backend_result = CreateRenderDriver();
		if (!backend_result) return backend_result;

		model_ = std::make_unique<GameModel>(
			create_info.world_size_chunks,
			&asset_paths_,
			render_driver_->TextureLoading(),
			create_info.terrain_generation,
			&render_driver_->Backend());
		input_router_.BindMouseLook(
			model_->MutableCamera(),
			engine_.RuntimeSettings().editor.is_settings_menu_open,
			engine_.RuntimeSettings().player.mouse_look);
		engine_.ConfigureCallbacks(window_, input_router_);

		if (create_info.has_custom_camera)
		{
			model_->MutableCamera().MoveTo(create_info.camera_position);
			model_->MutableCamera().TurnTo(create_info.camera_look_at);
		}
		RuntimeRenderFrame frame{ window_, *model_, controller_, editor_controller_,
			runtime_input_actions_, engine_.MutableRuntimeSettings(), frame_timer_ };
		render_driver_->ConfigureModel(frame, create_info);
		return EngineStartupResult::Success();
	}
}
