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
		return impl_->Run();
	}

	bool Engine::Start()
	{
		return impl_->Start();
	}

	EngineStartResult Engine::StartDetailed()
	{
		return impl_->StartDetailed();
	}

	bool Engine::Step()
	{
		return impl_->Step();
	}

	void Engine::Shutdown() noexcept
	{
		impl_->Shutdown();
	}

	void Engine::RequestStop() noexcept
	{
		impl_->RequestStop();
	}

	bool Engine::IsRunning() const noexcept
	{
		return impl_->IsRunning();
	}
}
