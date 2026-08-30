#pragma once

#include "VulkanGpuChunkRendererMeshState.h"
#include "VulkanGpuChunkRendererResourceState.h"
#include "VoxelRenderStyle.h"

namespace ve::rendering
{
	struct VulkanGpuChunkRendererState :
		VulkanGpuChunkRendererResourceState,
		VulkanGpuChunkRendererMeshState
	{
		VoxelRenderStyle render_style_{};
	};
}
