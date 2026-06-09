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
		const bool taa_ok = temporal_anti_aliasing_.Initialize({ settings.width, settings.height });
		const bool shadows_ok = shadows_.Initialize(settings.shadow_cascade_count, settings.shadow_resolution);
		const bool vxgi_ok = global_illumination_.Initialize({ settings.vxgi_resolution, 256.0f });
		if (!(deferred_ok && ao_ok && taa_ok && shadows_ok && vxgi_ok)) Release();
		return deferred_ok && ao_ok && taa_ok && shadows_ok && vxgi_ok;
	}

	/// Releases all owned pass resources.
	void VoxelRenderPipeline::Release()
	{
		shadows_.Release();
		temporal_anti_aliasing_.Release();
		ambient_occlusion_.Release();
		deferred_.Release();
		global_illumination_.Release();
	}

	/// Returns the deferred G-buffer pass.
	DeferredRenderer& VoxelRenderPipeline::Deferred() noexcept { return deferred_; }

	/// Returns the screen-space ambient occlusion pass.
	AmbientOcclusionPass& VoxelRenderPipeline::AmbientOcclusion() noexcept { return ambient_occlusion_; }

	/// Returns the temporal anti-aliasing resolve pass.
	TemporalAntiAliasingPass& VoxelRenderPipeline::TemporalAntiAliasing() noexcept { return temporal_anti_aliasing_; }

	/// Returns the cascaded shadow map pass.
	CascadedShadowMap& VoxelRenderPipeline::Shadows() noexcept { return shadows_; }

	/// Returns the voxel cone tracing radiance volume.
	VoxelConeTracing& VoxelRenderPipeline::GlobalIllumination() noexcept { return global_illumination_; }

	/// Returns the volumetric raymarching pass.
	VolumetricRaymarcher& VoxelRenderPipeline::Volumetrics() noexcept { return volumetrics_; }
}
