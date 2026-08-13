#include "EngineRuntime.h"

#include "Logger.h"
#include "RenderViewFactory.h"

#include <glm/glm.hpp>

#include <memory>

namespace ve::engine
{
	EngineStartupResult EngineRuntime::CreateRuntimeSystems()
	{
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		const EngineStartupResult backend_result = CreateRenderBackend();
		if (!backend_result) return backend_result;

		ve::rendering::VulkanBackend* vulkan_backend = ActiveVulkanBackend();
		view_ = RenderViewFactory::Create({ window_.GraphicsApi(), &asset_paths_, vulkan_backend });
		if (view_ == nullptr)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Render view creation failed");
			return EngineStartupResult::Failure(
				EngineStartupFailure::RenderViewCreationFailed,
				"Render view creation failed");
		}

		const auto texture_loading = window_.GraphicsApi() == ve::rendering::GraphicsApi::Vulkan
			? ve::blocks::BlockRegistry::TextureLoading::MetadataOnly
			: ve::blocks::BlockRegistry::TextureLoading::LoadTextures;
		model_ = std::make_unique<GameModel>(
			create_info.world_size_chunks,
			&asset_paths_,
			texture_loading,
			create_info.terrain_generation,
			backend_.get());
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
		else if (window_.GraphicsApi() == ve::rendering::GraphicsApi::Vulkan)
		{
			model_->MutableCamera().MoveTo(glm::vec3(90.0f, 58.0f, 124.0f));
			model_->MutableCamera().TurnTo(glm::vec3(78.0f, 53.0f, 91.0f));
			VE_LOG_CATEGORY_INFO(ve::log::category::Engine,
				"Vulkan runtime is using the voxel world model");
		}
		else if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			editor_controller_.Initialize(window_, engine_.MutableRuntimeSettings());
		}
		return EngineStartupResult::Success();
	}
}
