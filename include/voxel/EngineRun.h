#pragma once

#include "voxel/EngineConfig.h"

#include <memory>
#include <string>
#include <vector>

namespace voxel
{
	[[nodiscard]] EngineConfig DesertDemo();
	[[nodiscard]] EngineConfig AquaModelDemo();
	[[nodiscard]] EngineConfig SponzaAtriumDemo();
	[[nodiscard]] EngineConfig MinecraftDemo();
	[[nodiscard]] EngineConfig WorldEditDemo();
	[[nodiscard]] EngineConfig ShowcaseDemo();
	[[nodiscard]] EngineConfig ArcadeSnakeDemo();
	[[nodiscard]] EngineConfig ArcadePaddleDemo();
	[[nodiscard]] EngineConfig ArcadeBlocksDemo();
	[[nodiscard]] EngineConfig ArcadeInvadersDemo();
	[[nodiscard]] EngineConfig ArcadeMazeDemo();
	[[nodiscard]] EngineConfig DefaultConfig();

	enum class EngineStartFailure
	{
		None,
		InvalidConfiguration,
		RuntimeUnavailable,
		RuntimeStartupFailed
	};

	struct EngineStartResult
	{
		EngineStartFailure failure = EngineStartFailure::None;
		std::vector<std::string> issues;
		std::string message;

		[[nodiscard]] static EngineStartResult Success();
		[[nodiscard]] static EngineStartResult InvalidConfiguration(std::vector<std::string> issues);
		[[nodiscard]] static EngineStartResult RuntimeUnavailable(std::string message);
		[[nodiscard]] static EngineStartResult RuntimeStartupFailed(std::string message);
		[[nodiscard]] explicit operator bool() const noexcept;
	};

	class Engine
	{
	public:
		explicit Engine(EngineConfig config = {});
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) noexcept;
		Engine& operator=(Engine&&) noexcept;

		/** @return Process-style status after running the engine-owned main loop. */
		[[nodiscard]] int Run();

		/** @return True when runtime systems were initialized for an externally driven frame loop. */
		[[nodiscard]] bool Start();

		/** @return Structured startup status for hosts that need actionable failure details. */
		[[nodiscard]] EngineStartResult StartDetailed();

		/** @return True when one frame ran and another frame should be requested by the host. */
		[[nodiscard]] bool Step();

		/** Releases runtime systems created by Start(). */
		void Shutdown() noexcept;

		/** Requests the running loop to stop at the next frame boundary. */
		void RequestStop() noexcept;

		/** @return True when Start() succeeded and Shutdown() has not been called yet. */
		[[nodiscard]] bool IsRunning() const noexcept;

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;
	};

	[[nodiscard]] int Run(EngineConfig config = {});
	[[nodiscard]] int Run(WorldConfig world);
	[[nodiscard]] int RunScene(WorldConfig scene);
	[[nodiscard]] int RunDesertDemo();
	[[nodiscard]] int RunAquaModelDemo();
	[[nodiscard]] int RunSponzaAtriumDemo();
	[[nodiscard]] int RunMinecraftDemo();
	[[nodiscard]] int RunWorldEditDemo();
	[[nodiscard]] int RunShowcaseDemo();
	[[nodiscard]] int RunArcadeSnakeDemo();
	[[nodiscard]] int RunArcadePaddleDemo();
	[[nodiscard]] int RunArcadeBlocksDemo();
	[[nodiscard]] int RunArcadeInvadersDemo();
	[[nodiscard]] int RunArcadeMazeDemo();
}
