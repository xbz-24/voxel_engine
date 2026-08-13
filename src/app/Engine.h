#pragma once

#include "EngineApplicationState.h"

namespace ve::engine
{
	class EngineRuntime;
	class VoxelSandboxModule;
}

/** Coordinates engine settings, callbacks, and the runtime entry point. */
class EngineApplication : private ve::engine::EngineApplicationState
{
public:
	EngineApplication();
	explicit EngineApplication(ve::engine::EngineCreateInfo create_info);
	explicit EngineApplication(ve::engine::ValidatedEngineCreateInfo create_info);

	~EngineApplication();

	int Run();

	[[nodiscard]] bool Start();

	[[nodiscard]] ve::engine::EngineStartupResult StartDetailed();

	[[nodiscard]] bool Step();

	void Shutdown() noexcept;

	void RequestStop() noexcept;

	[[nodiscard]] bool IsRunning() const noexcept;

private:
	friend class ve::engine::EngineRuntime;
	friend class ve::engine::VoxelSandboxModule;

	[[nodiscard]] const ve::engine::EngineCreateInfo& CreateInfo() const noexcept;

	[[nodiscard]] bool HasValidCreateInfo() const noexcept;
};
