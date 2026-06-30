#include "RenderBackend.h"

#include "RenderBackendCatalog.h"
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
		return RenderBackendCatalog::Find(Api()).migration;
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
