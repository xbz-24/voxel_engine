#include "Engine.h"

#include "AssetPaths.h"
#include "GameController.h"
#include "GameModel.h"
#include "GameView.h"
#include "Logger.h"

#include <memory>

namespace
{
	constexpr int kDefaultWorldSizeChunks = 10;
}

class Engine::RuntimeContext
{
public:
	/** @param engine Engine services used to bootstrap runtime systems. @return True when ready. */
	bool Initialize(Engine& engine);
	/** @param engine Engine services used by per-frame systems. */
	void RunMainLoop(Engine& engine);

private:
	/** @param engine Engine services used by one frame. */
	void RunFrame(Engine& engine);
	/** Loads OpenGL-backed assets after a window context exists. */
	void LoadRuntimeAssets();

	Window window_{ "Voxel Engine v1.0.0" };
	CallbackContext callback_context_{ nullptr, nullptr, { 0.0, 0.0, true } };
	ve::assets::AssetPaths asset_paths_;
	std::unique_ptr<ve::engine::GameModel> model_;
	std::unique_ptr<ve::engine::GameView> view_;
	ve::engine::GameController controller_;
	ve::time::FrameTimer frame_timer_;
};

int Engine::Run()
{
	std::unique_ptr<RuntimeContext> runtime = CreateRuntimeContext();
	if (!runtime) return -1;
	runtime->RunMainLoop(*this);
	ReleaseRenderCaches();
	VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
	return 0;
}

std::unique_ptr<Engine::RuntimeContext> Engine::CreateRuntimeContext()
{
	auto runtime = std::make_unique<RuntimeContext>();
	if (!runtime->Initialize(*this)) return nullptr;
	return runtime;
}

bool Engine::RuntimeContext::Initialize(Engine& engine)
{
	callback_context_.isSettingsMenuOpen = &engine._runtimeSettings.isSettingsMenuOpen;
	if (!engine.InitializeWindow(window_)) return false;
	engine.ConfigureOpenGLState();
	asset_paths_ = ve::assets::ResolveFromSourceFile(engine._applicationSourceFilePath);
	engine.ConfigureRuntimeLogging(asset_paths_);
	model_ = std::make_unique<ve::engine::GameModel>(kDefaultWorldSizeChunks);
	callback_context_.camera = &model_->MutableCamera();
	engine.ConfigureCallbacks(window_, callback_context_);
	LoadRuntimeAssets();
	return true;
}

void Engine::RuntimeContext::LoadRuntimeAssets()
{
	view_ = std::make_unique<ve::engine::GameView>(asset_paths_);
}

void Engine::RuntimeContext::RunMainLoop(Engine& engine)
{
	while (!window_.ShouldClose())
	{
		RunFrame(engine);
	}
}

void Engine::RuntimeContext::RunFrame(Engine& engine)
{
	engine.UpdateProjectionIfWindowChanged(window_);
	frame_timer_.Tick();
	controller_.Update(engine, window_, *model_, view_->MutableBlockRegistry(), frame_timer_.DeltaSeconds());
	engine.Render3DWorld(window_, model_->MutableCamera(), view_->MutableSkyBox(), view_->MutablePlane(), view_->MutableSelectionCube(), view_->MutableBlockRegistry(), model_->MutableWorld(), model_->GetSelection());
	view_->MutableHudRenderer().Draw(engine.CreateHudFrame(window_, model_->GetCamera(), frame_timer_, model_->GetSelection(), view_->MutableBlockRegistry(), model_->GetWorld()));
	window_.Update();
}
