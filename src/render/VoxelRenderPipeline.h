#pragma once

#include "AmbientOcclusionPass.h"
#include "CascadedShadowMap.h"
#include "DeferredRenderer.h"
#include "FrameGraph.h"
#include "GpuDrivenRenderer.h"
#include "TemporalAntiAliasingPass.h"
#include "VolumetricRaymarcher.h"
#include "VoxelConeTracing.h"

#include <cstddef>

namespace ve::rendering
{
	struct VoxelRenderPipelineSettings
	{
		int width = 1;
		int height = 1;
		std::size_t shadow_cascade_count = 4;
		int shadow_resolution = 2048;
		int vxgi_resolution = 128;
	};

	class VoxelRenderPipeline
	{
	public:
		~VoxelRenderPipeline();

		/** @param settings Framebuffer and shadow-map settings. @return True when every pass initializes. */
		[[nodiscard]] bool Initialize(const VoxelRenderPipelineSettings& settings);

		/** Releases all owned pass resources. */
		void Release();

		/** @return Deferred G-buffer pass. */
		[[nodiscard]] DeferredRenderer& Deferred() noexcept;

		/** @return Frame graph used to schedule render passes. */
		[[nodiscard]] FrameGraph& Graph() noexcept;

		/** @return GPU-driven indirect draw submission API. */
		[[nodiscard]] GpuDrivenRenderer& GpuDriven() noexcept;

		/** @return Screen-space ambient occlusion pass. */
		[[nodiscard]] AmbientOcclusionPass& AmbientOcclusion() noexcept;

		/** @return Temporal anti-aliasing resolve pass. */
		[[nodiscard]] TemporalAntiAliasingPass& TemporalAntiAliasing() noexcept;

		/** @return Cascaded shadow map pass. */
		[[nodiscard]] CascadedShadowMap& Shadows() noexcept;

		/** @return Voxel cone tracing radiance volume. */
		[[nodiscard]] VoxelConeTracing& GlobalIllumination() noexcept;

		/** @return Volumetric raymarching pass. */
		[[nodiscard]] VolumetricRaymarcher& Volumetrics() noexcept;

	private:
		DeferredRenderer deferred_;
		FrameGraph frame_graph_;
		GpuDrivenRenderer gpu_driven_;
		AmbientOcclusionPass ambient_occlusion_;
		TemporalAntiAliasingPass temporal_anti_aliasing_;
		CascadedShadowMap shadows_;
		VoxelConeTracing global_illumination_;
		VolumetricRaymarcher volumetrics_;
	};
}
