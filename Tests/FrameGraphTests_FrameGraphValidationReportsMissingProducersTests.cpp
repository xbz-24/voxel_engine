#include <doctest/doctest.h>

#include "AmbientOcclusionPass.h"
#include "FrameGraph.h"
#include "TemporalAntiAliasingPass.h"

#include <string>
#include <utility>
#include <vector>

TEST_CASE("frame graph validation reports missing producers")
{
	ve::rendering::FrameGraph graph;
	const ve::rendering::FrameGraphResourceHandle imported = graph.DeclareResource("history");

	ve::rendering::FrameGraphPass pass{};
	pass.name = "temporal resolve";
	pass.reads = { imported };
	graph.AddPass(std::move(pass));

	const ve::core::DynamicArray<std::string> issues = graph.Validate();

	REQUIRE(issues.size() == 1U);
	CHECK(issues.front() == "FrameGraph pass 'temporal resolve' reads resource 'history' without a producer");
}

