#include "VoxelSandboxModule.h"

#include "Engine.h"
#include "GameModel.h"
#include "Logger.h"
#include "RuntimeRenderDriver.h"
#include "RuntimeRenderDriverFactory.h"
#include "RuntimeRenderFrame.h"
#include "Window.h"

#include <memory>

namespace ve::engine
{
	EngineStartupResult VoxelSandboxModule::CreateRenderDriver(RuntimeModuleContext& context)
	{
		render_driver_ = RuntimeRenderDriverFactory::Create(context.window.GraphicsApi());
		if (!render_driver_)
		{
			const EngineStartupResult result = RuntimeRenderDriverFactory::MissingDriverResult(
				context.window.GraphicsApi());
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, result.message);
			return result;
		}
		const EngineStartupResult result = render_driver_->Initialize(
			context.window, context.assets, engine_.CreateInfo());
		if (!result) VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, result.message);
		return result;
	}

	EngineStartupResult VoxelSandboxModule::CreateStaticModelScene()
	{
		const auto& configuration = engine_.CreateInfo().static_model_scene;
		if (!configuration) return EngineStartupResult::Success();
		return static_model_scene_.Initialize(*configuration, render_driver_->Backend());
	}

	EngineStartupResult VoxelSandboxModule::Initialize(RuntimeModuleContext& context)
	{
		window_ = &context.window;
		frame_timer_ = &context.frame_timer;
		const EngineStartupResult driver_result = CreateRenderDriver(context);
		if (!driver_result) return driver_result;
		const EngineStartupResult static_model_result = CreateStaticModelScene();
		if (!static_model_result) return static_model_result;
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		model_ = std::make_unique<GameModel>(create_info.world_size_chunks,
			&context.assets, render_driver_->TextureLoading(), create_info.terrain_generation,
			&render_driver_->Backend());
		input_router_.BindMouseLook(model_->MutableCamera(),
			runtime_settings_.editor.is_settings_menu_open, runtime_settings_.player.mouse_look);
		ConfigureInputCallbacks();
		if (create_info.has_custom_camera)
		{
			model_->MutableCamera().MoveTo(create_info.camera_position);
			model_->MutableCamera().TurnTo(create_info.camera_look_at);
		}
		RuntimeRenderFrame frame{ context.window, *model_, controller_, editor_controller_,
			runtime_input_actions_, runtime_settings_, context.frame_timer };
		render_driver_->ConfigureModel(frame, create_info);
		return EngineStartupResult::Success();
	}
}
