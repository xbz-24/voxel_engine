#include "VoxelSandboxModule.h"

#include "CoreTypes.h"
#include "RenderState.h"

namespace
{
	constexpr float FarWorldClipDistance = 1024.0f;
}

/// Rebuilds 3D and 2D projection matrices after window size changes.
void ve::engine::VoxelSandboxModule::UpdateProjections(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	const float aspect = ve::core::ToFloat(width) / ve::core::ToFloat(height);
	render_cache_state_.projection_3d = glm::frustum(
		-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, FarWorldClipDistance);
	ve::rendering::SetViewport(width, height);
}
