#include "AmbientOcclusionPass.h"

#include "CoreTypes.h"

#include <algorithm>

namespace
{
	ve::core::Index ResourceExtent(int value) noexcept
	{
		return ve::core::ToIndex(std::max(1, value));
	}
}

namespace ve::rendering
{
	AmbientOcclusionResources AmbientOcclusionPass::DescribeResources(int width, int height)
	{
		AmbientOcclusionResources resources{};
		resources.depth_input.name = "ao.depth";
		resources.depth_input.format = FrameGraphResourceFormat::Depth24Stencil8;
		resources.depth_input.width = ResourceExtent(width);
		resources.depth_input.height = ResourceExtent(height);
		resources.depth_input.lifetime = FrameGraphResourceLifetime::Imported;
		resources.depth_input.imported = true;

		resources.occlusion_output.name = "ao.occlusion";
		resources.occlusion_output.format = FrameGraphResourceFormat::R8;
		resources.occlusion_output.width = ResourceExtent(width);
		resources.occlusion_output.height = ResourceExtent(height);
		resources.occlusion_output.lifetime = FrameGraphResourceLifetime::Exported;
		resources.occlusion_output.exported = true;
		return resources;
	}

	const AmbientOcclusionResources& AmbientOcclusionPass::Resources() const noexcept
	{
		return resources_;
	}
}
