#pragma once

#include "AssetPaths.h"
#include "FrameTimer.h"
#include "RuntimeHostConfiguration.h"
#include "RuntimeModule.h"
#include "Window.h"

#include <atomic>
#include <memory>

namespace ve::engine
{
	/** Owns the active runtime systems created by EngineApplication::Run. */
	class EngineRuntime
	{
	public:
		EngineRuntime(RuntimeHostConfiguration configuration,
			std::unique_ptr<IRuntimeModule> module);

		/** Initializes systems, runs the frame loop, shuts down, and returns a process status. */
		[[nodiscard]] int Execute();

		/** Initializes all runtime systems without entering the main loop. */
		[[nodiscard]] EngineStartupResult Start();

		/** Runs exactly one frame and reports whether another frame should be requested. */
		[[nodiscard]] bool Step();

		/** Releases runtime systems and returns the object to an unstarted state. */
		void Shutdown();
		void RequestStop() noexcept;
		[[nodiscard]] bool IsStopRequested() const noexcept;

	private:
		[[nodiscard]] EngineStartupResult Initialize();
		void PrepareAssetsAndLogging();
		[[nodiscard]] EngineStartupResult InitializeWindow();
		void RunMainLoop();
		void RunFrame();
		[[nodiscard]] bool ShouldContinue() const noexcept;
		void EndRuntimeFrame();

		RuntimeHostConfiguration configuration_;
		std::unique_ptr<IRuntimeModule> module_;
		Window window_;
		ve::assets::AssetPaths asset_paths_;
		ve::time::FrameTimer frame_timer_;
		std::atomic_bool stop_requested_{ false };
	};
}
