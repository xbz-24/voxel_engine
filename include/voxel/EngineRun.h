#pragma once

#include "voxel/EngineConfig.h"

#include <memory>

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

	class Engine
	{
	public:
		// TODO: Expose Pause/Step/Stop hooks so embedding apps can drive the engine without surrendering the main loop.
		explicit Engine(EngineConfig config = {});
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) noexcept;
		Engine& operator=(Engine&&) noexcept;

		[[nodiscard]] int Run();
		void RequestStop() noexcept;

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
