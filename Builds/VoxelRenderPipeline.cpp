#include "VoxelRenderPipeline.h"

namespace ve::rendering
{
	/// Releases all render pass resources.
	VoxelRenderPipeline::~VoxelRenderPipeline()
	{
		Release();
	}

	/// Initializes deferred, AO and shadow resources as one high-level pipeline object.
	bool VoxelRenderPipeline::Initialize(const VoxelRenderPipelineSettings& settings)
	{
		Release();
		const bool deferred_ok = deferred_.Initialize({ settings.width, settings.height });
		const bool ao_ok = ambient_occlusion_.Initialize({ settings.width, settings.height });
		const bool shadows_ok = shadows_.Initialize(settings.shadow_cascade_count, settings.shadow_resolution);
		if (!(deferred_ok && ao_ok && shadows_ok)) Release();
		return deferred_ok && ao_ok && shadows_ok;
	}

	/// Releases all owned pass resources.
	void VoxelRenderPipeline::Release()
	{
		shadows_.Release();
		ambient_occlusion_.Release();
		deferred_.Release();
	}

	/// Returns the deferred G-buffer pass.
	DeferredRenderer& VoxelRenderPipeline::Deferred() noexcept { return deferred_; }

	/// Returns the screen-space ambient occlusion pass.
	AmbientOcclusionPass& VoxelRenderPipeline::AmbientOcclusion() noexcept { return ambient_occlusion_; }

	/// Returns the cascaded shadow map pass.
	CascadedShadowMap& VoxelRenderPipeline::Shadows() noexcept { return shadows_; }
}
