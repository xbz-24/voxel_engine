#include "voxel/Engine.h"

#include "EngineInstanceImpl.h"
#include <memory>
#include <utility>

namespace voxel
{
	Engine::Engine(EngineConfig config)
		: impl_(std::make_unique<Impl>(config))
	{
	}

	Engine::~Engine() = default;

	Engine::Engine(Engine&&) noexcept = default;

	Engine& Engine::operator=(Engine&&) noexcept = default;

	int Engine::Run()
	{
		return impl_ ? impl_->Run() : -1;
	}

	bool Engine::Start()
	{
		return impl_ && impl_->Start();
	}

	EngineStartResult Engine::StartDetailed()
	{
		return impl_ ? impl_->StartDetailed() :
			EngineStartResult::RuntimeUnavailable("Engine instance was moved from");
	}

	bool Engine::Step()
	{
		return impl_ && impl_->Step();
	}

	void Engine::Shutdown() noexcept
	{
		if (impl_) impl_->Shutdown();
	}

	void Engine::RequestStop() noexcept
	{
		if (impl_) impl_->RequestStop();
	}

	bool Engine::IsRunning() const noexcept
	{
		return impl_ && impl_->IsRunning();
	}
}
