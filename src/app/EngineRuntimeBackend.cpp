#include "EngineRuntime.h"
#include "EngineRuntimeEnvironment.h"

#include "Logger.h"
#include "RenderBackendFactory.h"
#include "VulkanBackend.h"

#include <cassert>

namespace ve::engine
{
	ve::rendering::VulkanBackend* EngineRuntime::ActiveVulkanBackend() noexcept
	{
		if (backend_ == nullptr || backend_->Api() != ve::rendering::GraphicsApi::Vulkan) return nullptr;
		return static_cast<ve::rendering::VulkanBackend*>(backend_.get());
	}

	ve::rendering::VulkanBackend& EngineRuntime::RequiredVulkanBackend() noexcept
	{
		ve::rendering::VulkanBackend* vulkan_backend = ActiveVulkanBackend();
		assert(vulkan_backend != nullptr);
		return *vulkan_backend;
	}

	EngineStartupResult EngineRuntime::CreateRenderBackend()
	{
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		backend_ = ve::rendering::RenderBackendFactory::Create(window_.GraphicsApi());
		if (backend_ == nullptr)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::RenderBackendUnavailable,
				"Render backend factory returned no backend");
		}
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility)
			return EngineStartupResult::Success();
		if (window_.GraphicsApi() != ve::rendering::GraphicsApi::Vulkan)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine,
				"Selected render backend is not implemented");
			return EngineStartupResult::Failure(
				EngineStartupFailure::UnsupportedRenderBackend,
				"Selected render backend is not implemented");
		}

		ve::rendering::VulkanBackend& vulkan_backend = RequiredVulkanBackend();
		ve::rendering::VulkanBackendSettings settings{};
#if !defined(NDEBUG)
		if (runtime_detail::EnvironmentFlagEnabled("VE_VULKAN_VALIDATION"))
		{
			settings.context.enable_validation_layers = true;
			settings.context.enable_debug_utils = true;
		}
#endif
		const auto result = vulkan_backend.InitializeDetailed(settings, window_);
		if (!result)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine,
				"Vulkan backend initialization failed: " + result.message);
			return EngineStartupResult::Failure(
				EngineStartupFailure::RenderBackendInitializationFailed,
				"Vulkan backend initialization failed: " + result.message);
		}
		if (vulkan_frame_orchestrator_.Initialize(vulkan_backend,
			window_,
			asset_paths_.blockTexturesDirectory,
			create_info.voxel_render_style,
			create_info.show_debug_overlay && create_info.settings_menu_enabled))
		{
			return EngineStartupResult::Success();
		}
		VE_LOG_CATEGORY_ERROR(ve::log::category::Engine,
			"Vulkan frame orchestrator initialization failed");
		return EngineStartupResult::Failure(
			EngineStartupFailure::RenderFrameRendererInitializationFailed,
			"Vulkan frame orchestrator initialization failed");
	}
}
