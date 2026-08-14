#include "VulkanRuntimeRenderDriver.h"

#include "EngineConfiguration.h"
#include "EngineRuntimeEnvironment.h"
#include "GameModel.h"
#include "Logger.h"
#include "RenderView.h"
#include "RenderViewFactory.h"
#include "RuntimeRenderFrame.h"
#include "VulkanBackend.h"

#include <cassert>
#include <glm/glm.hpp>

namespace ve::engine
{
	VulkanRuntimeRenderDriver::~VulkanRuntimeRenderDriver() = default;

	EngineStartupResult VulkanRuntimeRenderDriver::Initialize(Window& window,
		const ve::assets::AssetPaths& assets,
		const EngineCreateInfo& create_info)
	{
		backend_ = std::make_unique<ve::rendering::VulkanBackend>();
		ve::rendering::VulkanBackendSettings settings{};
#if !defined(NDEBUG)
		if (runtime_detail::EnvironmentFlagEnabled("VE_VULKAN_VALIDATION"))
		{
			settings.context.enable_validation_layers = true;
			settings.context.enable_debug_utils = true;
		}
#endif
		const auto result = backend_->InitializeDetailed(settings, window);
		if (!result) return EngineStartupResult::Failure(
			EngineStartupFailure::RenderBackendInitializationFailed,
			"Vulkan backend initialization failed: " + result.message);
		if (!orchestrator_.Initialize(*backend_, window, assets.blockTexturesDirectory,
			assets.vulkanShaderDirectory,
			create_info.voxel_render_style,
			create_info.show_debug_overlay && create_info.settings_menu_enabled))
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::RenderFrameRendererInitializationFailed,
				"Vulkan frame orchestrator initialization failed");
		}
		view_ = RenderViewFactory::Create({ backend_->Api(), &assets, backend_.get() });
		if (!view_) return EngineStartupResult::Failure(
			EngineStartupFailure::RenderViewCreationFailed, "Render view creation failed");
		return EngineStartupResult::Success();
	}

	ve::rendering::RenderBackend& VulkanRuntimeRenderDriver::Backend() noexcept
	{
		assert(backend_ != nullptr);
		return *backend_;
	}

	ve::rendering::RenderBackendCapabilities VulkanRuntimeRenderDriver::Capabilities() const noexcept
	{
		return backend_ ? backend_->Capabilities() : ve::rendering::RenderBackendCapabilities{};
	}

	ve::blocks::BlockRegistry::TextureLoading VulkanRuntimeRenderDriver::TextureLoading() const noexcept
	{
		return ve::blocks::BlockRegistry::TextureLoading::MetadataOnly;
	}

	void VulkanRuntimeRenderDriver::ConfigureModel(RuntimeRenderFrame& frame,
		const EngineCreateInfo& create_info)
	{
		if (create_info.has_custom_camera) return;
		frame.model.MutableCamera().MoveTo(glm::vec3(90.0f, 58.0f, 124.0f));
		frame.model.MutableCamera().TurnTo(glm::vec3(78.0f, 53.0f, 91.0f));
		VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Vulkan runtime is using the voxel world model");
	}

	void VulkanRuntimeRenderDriver::Shutdown() noexcept
	{
		if (view_) view_->ReleaseCachedResources();
		view_.reset();
		orchestrator_.Release();
		if (backend_) backend_->Release();
		backend_.reset();
	}
}
