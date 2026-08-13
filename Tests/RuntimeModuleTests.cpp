#include <doctest/doctest.h>

#include "AssetPaths.h"
#include "EngineRuntime.h"
#include "FrameTimer.h"
#include "RuntimeModule.h"
#include "Window.h"

#include <memory>
#include <utility>

namespace
{
	class RecordingRuntimeModule final : public ve::engine::IRuntimeModule
	{
	public:
		ve::engine::EngineStartupResult Initialize(
			ve::engine::RuntimeModuleContext& context) override
		{
			initialized_window = &context.window;
			return ve::engine::EngineStartupResult::Success();
		}

		void RunFrame(ve::engine::RuntimeModuleContext& context) override
		{
			frame_assets = &context.assets;
			++frame_count;
		}

		void Shutdown() noexcept override { shutdown_called = true; }

		ve::engine::Window* initialized_window = nullptr;
		const ve::assets::AssetPaths* frame_assets = nullptr;
		int frame_count = 0;
		bool shutdown_called = false;
	};
}

TEST_CASE("runtime module receives generic host services without graphics startup")
{
	ve::engine::Window window{ ve::engine::WindowCreateInfo{} };
	ve::assets::AssetPaths assets{};
	ve::time::FrameTimer frame_timer{};
	ve::engine::RuntimeModuleContext context{ window, assets, frame_timer };
	RecordingRuntimeModule module;

	CHECK(module.Initialize(context));
	module.RunFrame(context);
	module.Shutdown();

	CHECK(module.initialized_window == &window);
	CHECK(module.frame_assets == &assets);
	CHECK(module.frame_count == 1);
	CHECK(module.shutdown_called);
}

TEST_CASE("runtime host owns stop state without graphics startup")
{
	ve::engine::RuntimeHostConfiguration configuration{};
	auto module = std::make_unique<RecordingRuntimeModule>();
	ve::engine::EngineRuntime runtime(std::move(configuration), std::move(module));

	CHECK_FALSE(runtime.IsStopRequested());
	runtime.RequestStop();
	CHECK(runtime.IsStopRequested());
}
