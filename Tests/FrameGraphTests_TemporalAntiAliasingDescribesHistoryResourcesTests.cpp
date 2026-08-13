#include <doctest/doctest.h>

#include "AmbientOcclusionPass.h"
#include "FrameGraph.h"
#include "TemporalAntiAliasingPass.h"

#include <string>
#include <utility>
#include <vector>

TEST_CASE("temporal anti aliasing describes history resources")
{
	const ve::rendering::TemporalHistoryResources resources =
		ve::rendering::TemporalAntiAliasingPass::DescribeHistoryResources(1920, 1080);

	CHECK(resources.previous_history.name == "taa.previous_history");
	CHECK(resources.previous_history.format == ve::rendering::FrameGraphResourceFormat::Rgba16Float);
	CHECK(resources.previous_history.imported);
	CHECK(resources.current_history.name == "taa.current_history");
	CHECK(resources.current_history.width == 1920U);
	CHECK(resources.current_history.height == 1080U);
	CHECK(resources.current_history.exported);
}
