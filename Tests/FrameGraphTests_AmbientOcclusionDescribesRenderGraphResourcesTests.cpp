#include <doctest/doctest.h>

#include "AmbientOcclusionPass.h"
#include "FrameGraph.h"
#include "TemporalAntiAliasingPass.h"

#include <string>
#include <utility>
#include <vector>

TEST_CASE("ambient occlusion describes render graph resources")
{
	const ve::rendering::AmbientOcclusionResources resources =
		ve::rendering::AmbientOcclusionPass::DescribeResources(640, 360);

	CHECK(resources.depth_input.name == "ao.depth");
	CHECK(resources.depth_input.format == ve::rendering::FrameGraphResourceFormat::Depth24Stencil8);
	CHECK(resources.depth_input.width == 640U);
	CHECK(resources.depth_input.height == 360U);
	CHECK(resources.depth_input.imported);
	CHECK(resources.occlusion_output.name == "ao.occlusion");
	CHECK(resources.occlusion_output.format == ve::rendering::FrameGraphResourceFormat::R8);
	CHECK(resources.occlusion_output.exported);
}

