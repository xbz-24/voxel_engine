#include "RenderBackend.h"

#include "RenderBackendSelector.h"

namespace ve::rendering
{
	/** Returns the display name for the concrete backend API. */
	const char* RenderBackend::Name() const noexcept
	{
		return RenderBackendSelector::Name(Api());
	}

	RenderBackendMigrationStatus RenderBackend::MigrationStatus() const noexcept
	{
		// TODO: Feed this from backend self-tests once runtime smoke coverage exists for every API.
		return {};
	}

	std::unique_ptr<RenderMesh> RenderBackend::CreateMeshResource() const
	{
		return nullptr;
	}

	const ComputeDispatcher* RenderBackend::Compute() const noexcept
	{
		return nullptr;
	}
}
