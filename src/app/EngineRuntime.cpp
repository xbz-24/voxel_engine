#include "EngineRuntime.h"

#include "Logger.h"
#include "OpenGLRenderView.h"
#include "RenderBackendFactory.h"
#include "RenderViewFactory.h"
#include "VulkanBackend.h"

#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <glm/glm.hpp>

namespace
{
#if !defined(NDEBUG)
	bool EnvironmentFlagEnabled(const char* name) noexcept
	{
#if defined(_MSC_VER)
		char* value = nullptr;
		std::size_t value_size = 0;
		if (_dupenv_s(&value, &value_size, name) != 0 || value == nullptr) return false;
		const bool enabled = std::strcmp(value, "1") == 0;
		std::free(value);
		return enabled;
#else
		const char* value = std::getenv(name);
		return value != nullptr && std::strcmp(value, "1") == 0;
#endif
	}
#endif
}

namespace ve::engine
{
	/** Stores the engine facade used by runtime systems. */
	EngineRuntime::EngineRuntime(EngineApplication& engine) noexcept
		: engine_(engine),
		  window_(engine.create_info_.window)
	{
	}

	/** Initializes, runs, and shuts down the runtime. */
	int EngineRuntime::Execute()
	{
		if (!Initialize())
		{
			Shutdown();
			return -1;
		}
		RunMainLoop();
		Shutdown();
		return 0;
	}

	/** Creates the window and every runtime system needed by the frame loop. */
	bool EngineRuntime::Initialize()
	{
		callback_context_.isSettingsMenuOpen = &engine_._runtimeSettings.isSettingsMenuOpen;
		PrepareAssetsAndLogging();
		if (!engine_.InitializeWindow(window_)) return false;
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility) engine_.ConfigureOpenGLState();
		return CreateRuntimeSystems();
	}

	/** Resolves asset paths and configures logger sinks. */
	void EngineRuntime::PrepareAssetsAndLogging()
	{
		asset_paths_ = ve::assets::Resolve();
		engine_.ConfigureRuntimeLogging(asset_paths_);
	}

	/** Creates model, callbacks, editor UI, and view resources. */
	bool EngineRuntime::CreateRuntimeSystems()
	{
		if (!CreateRenderBackend()) return false;
		ve::rendering::VulkanBackend* vulkan_backend = nullptr;
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::Vulkan)
		{
			vulkan_backend = static_cast<ve::rendering::VulkanBackend*>(backend_.get());
		}
		view_ = RenderViewFactory::Create({ window_.GraphicsApi(), &asset_paths_, vulkan_backend });
		if (view_ == nullptr)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Render view creation failed");
			return false;
		}
		const int world_size_chunks = engine_.create_info_.world_size_chunks;
		model_ = std::make_unique<GameModel>(world_size_chunks, &asset_paths_);
		callback_context_.camera = &model_->MutableCamera();
		engine_.ConfigureCallbacks(window_, callback_context_);
		if (engine_.create_info_.has_custom_camera)
		{
			model_->MutableCamera().MoveTo(engine_.create_info_.camera_position);
			model_->MutableCamera().TurnTo(engine_.create_info_.camera_look_at);
		}
		else if (window_.GraphicsApi() == ve::rendering::GraphicsApi::Vulkan)
		{
			model_->MutableCamera().MoveTo(glm::vec3(90.0f, 58.0f, 124.0f));
			model_->MutableCamera().TurnTo(glm::vec3(78.0f, 53.0f, 91.0f));
			VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Vulkan runtime is using the voxel world model");
		}
		else if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			editor_controller_.Initialize(window_, engine_._runtimeSettings);
		}
		return true;
	}

	/** Creates and initializes the selected backend before constructing its view. */
	bool EngineRuntime::CreateRenderBackend()
	{
		backend_ = ve::rendering::RenderBackendFactory::Create(window_.GraphicsApi());
		if (backend_ == nullptr) return false;
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility) return true;
		if (window_.GraphicsApi() != ve::rendering::GraphicsApi::Vulkan)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Selected render backend is not implemented");
			return false;
		}

		auto& vulkan_backend = static_cast<ve::rendering::VulkanBackend&>(*backend_);
		ve::rendering::VulkanBackendSettings settings{};
#if !defined(NDEBUG)
		if (EnvironmentFlagEnabled("VE_VULKAN_VALIDATION"))
		{
			settings.context.enable_validation_layers = true;
			settings.context.enable_debug_utils = true;
		}
#endif
		if (!vulkan_backend.Initialize(settings, window_))
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Vulkan backend initialization failed");
			return false;
		}
		if (!vulkan_frame_renderer_.Initialize(vulkan_backend, window_, asset_paths_.blockTexturesDirectory))
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Vulkan frame renderer initialization failed");
			return false;
		}
		return true;
	}

	void EngineRuntime::ApplyConfiguredWorldEditsOnce()
	{
		if (configured_world_edits_applied_ || model_ == nullptr) return;
		if (engine_.create_info_.world_edits.empty())
		{
			configured_world_edits_applied_ = true;
			return;
		}
		if (model_->PendingWorldGenerationCount() != 0) return;

		ve::world::World& world = model_->MutableWorld();
		for (const WorldBlockEdit& edit : engine_.create_info_.world_edits)
		{
			const int min_x = std::min(edit.min_x, edit.max_x);
			const int min_y = std::min(edit.min_y, edit.max_y);
			const int min_z = std::min(edit.min_z, edit.max_z);
			const int max_x = std::max(edit.min_x, edit.max_x);
			const int max_y = std::max(edit.min_y, edit.max_y);
			const int max_z = std::max(edit.min_z, edit.max_z);

			for (int z = min_z; z <= max_z; ++z)
			{
				for (int y = min_y; y <= max_y; ++y)
				{
					for (int x = min_x; x <= max_x; ++x)
					{
						(void)world.SetBlock(x, y, z, edit.block);
					}
				}
			}
		}
		configured_world_edits_applied_ = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::World, "Applied configured public API world edits");
	}

	/** Returns the OpenGL compatibility view after startup validation. */
	OpenGLRenderView& EngineRuntime::LegacyOpenGLView() noexcept
	{
		OpenGLRenderView* legacy_view = view_ ? TryRenderViewCast<OpenGLRenderView>(*view_) : nullptr;
		assert(legacy_view != nullptr);
		return *legacy_view;
	}

	/** Runs frames until the window asks to close. */
	void EngineRuntime::RunMainLoop()
	{
		while (!window_.ShouldClose()) RunFrame();
	}

	/** Releases runtime resources and reports shutdown. */
	void EngineRuntime::Shutdown()
	{
		editor_controller_.Shutdown();
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility) engine_.ReleaseRenderCaches();
		view_.reset();
		vulkan_frame_renderer_.Release();
		if (backend_ == nullptr)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Engine, "No render backend to release");
			model_.reset();
			VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
			return;
		}
		if (backend_->Api() == ve::rendering::GraphicsApi::Vulkan)
		{
			static_cast<ve::rendering::VulkanBackend&>(*backend_).Release();
		}
		backend_.reset();
		model_.reset();
		VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
	}

	void VulkanInputController::Update(GLFWwindow* window)
	{
		previous_state_ = current_state_;

		current_state_.set(static_cast<size_t>(Action::LeftClick), glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		current_state_.set(static_cast<size_t>(Action::F1), glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS);
		current_state_.set(static_cast<size_t>(Action::F2), glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS);
	}

	bool VulkanInputController::IsDown(Action action) const
	{
		return current_state_[static_cast<size_t>(action)];
	}

	bool VulkanInputController::IsJustPressed(Action action) const
	{
		return current_state_[static_cast<size_t>(action)] && !previous_state_[static_cast<size_t>(action)];
	}
}
