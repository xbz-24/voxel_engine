#pragma once


#include "voxel/EngineConfig.h"

#include <memory>
#include <string>
#include <vector>

namespace voxel
{
	/** @addtogroup voxel_sdk_startup
	 * @{
	 */
	/** Structured startup failure reason for EngineStartResult. */
	enum class EngineStartFailure
	{
		None,
		InvalidConfiguration,
		RuntimeUnavailable,
		RuntimeStartupFailed
	};

	/** Structured result returned by Engine::StartDetailed. */
	struct EngineStartResult
	{
		EngineStartFailure failure = EngineStartFailure::None;
		std::vector<std::string> issues;
		std::string message;

		/** Creates a successful startup result. */
		[[nodiscard]] static EngineStartResult Success();

		/** Creates a validation-failure startup result with public issue messages. */
		[[nodiscard]] static EngineStartResult InvalidConfiguration(std::vector<std::string> issues);

		/** Creates a startup result for missing runtime capabilities. */
		[[nodiscard]] static EngineStartResult RuntimeUnavailable(std::string message);

		/** Creates a startup result for runtime initialization failure. */
		[[nodiscard]] static EngineStartResult RuntimeStartupFailed(std::string message);

		/** Returns true only for successful startup. */
		[[nodiscard]] explicit operator bool() const noexcept;
	};

	/** Owns an engine runtime instance for either self-driven or host-driven loops. */
	class Engine
	{
	public:
		/** Creates an engine wrapper with a copied public configuration. */
		explicit Engine(EngineConfig config = {});

		/** Releases any active runtime before destroying the wrapper. */
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

	/** Runs the engine-owned main loop and returns a process-style exit code. */
	[[nodiscard]] int Run(EngineConfig config = {});

	/** Runs a world config using default engine settings. */
	[[nodiscard]] int Run(WorldConfig world);

	/** Runs a scene/world config using default engine settings. */
	[[nodiscard]] int RunScene(WorldConfig scene);
	/** @} */
}
