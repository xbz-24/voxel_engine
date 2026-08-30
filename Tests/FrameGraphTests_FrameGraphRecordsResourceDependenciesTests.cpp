#include <doctest/doctest.h>

#include "AmbientOcclusionPass.h"
#include "FrameGraph.h"
#include "TemporalAntiAliasingPass.h"

#include <string>
#include <utility>
#include <vector>

TEST_CASE("frame graph records resource dependencies")
{
	ve::rendering::FrameGraph graph;
	ve::rendering::FrameGraphResourceDescriptor color_descriptor{};
	color_descriptor.name = "color";
	color_descriptor.format = ve::rendering::FrameGraphResourceFormat::Rgba16Float;
	color_descriptor.width = 1280;
	color_descriptor.height = 720;
	const ve::rendering::FrameGraphResourceHandle color = graph.DeclareResource(std::move(color_descriptor));
	const ve::rendering::FrameGraphResourceHandle depth = graph.DeclareResource("depth");

	ve::rendering::FrameGraphPass pass{};
	pass.name = "geometry";
	pass.writes = { color, depth };
	graph.AddPass(std::move(pass));

	REQUIRE(graph.ResourceCount() == 2U);
	REQUIRE(graph.PassCount() == 1U);
	CHECK(graph.Resources().front().name == "color");
	CHECK(graph.Resources().front().format == ve::rendering::FrameGraphResourceFormat::Rgba16Float);
	CHECK(graph.Resources().front().width == 1280U);
	CHECK(graph.Resources().front().height == 720U);
	CHECK(graph.Passes().front().writes.size() == 2U);
	CHECK(graph.Passes().front().writes.front() == color);
}

