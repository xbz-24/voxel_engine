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

	RenderTexturePtr RenderBackend::CreateTextureResource(const RenderTextureDescriptor&) const
	{
		return nullptr;
	}

	RenderTexturePtr RenderBackend::CreateTextureResource(const RenderTextureUpload&) const
	{
		return nullptr;
	}

	RenderMaterialPtr RenderBackend::CreateMaterialResource(const RenderMaterialDescriptor&) const
	{
		return nullptr;
	}

	RenderTargetPtr RenderBackend::CreateRenderTarget(const RenderTargetDescriptor&) const
	{
		return nullptr;
	}

	RenderSceneInstancePtr RenderBackend::CreateSceneInstance(const RenderSceneInstanceDescriptor&) const
	{
		return nullptr;
	}

	void RenderBackend::BeginFrame()
	{
	}

	void RenderBackend::EndFrame()
	{
	}

	const ComputeDispatcher* RenderBackend::Compute() const noexcept
	{
		return nullptr;
	}
}
