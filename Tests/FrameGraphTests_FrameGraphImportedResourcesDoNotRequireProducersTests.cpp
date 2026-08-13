#include <doctest/doctest.h>

#include "AmbientOcclusionPass.h"
#include "FrameGraph.h"
#include "TemporalAntiAliasingPass.h"

#include <string>
#include <utility>
#include <vector>

TEST_CASE("frame graph imported resources do not require producers")
{
	ve::rendering::FrameGraph graph;
	ve::rendering::FrameGraphResourceDescriptor history_descriptor{};
	history_descriptor.name = "history";
	history_descriptor.format = ve::rendering::FrameGraphResourceFormat::Rgba16Float;
	history_descriptor.lifetime = ve::rendering::FrameGraphResourceLifetime::Imported;
	const ve::rendering::FrameGraphResourceHandle history = graph.DeclareResource(std::move(history_descriptor));

	ve::rendering::FrameGraphPass pass{};
	pass.name = "temporal resolve";
	pass.reads = { history };
	graph.AddPass(std::move(pass));

	CHECK(graph.Validate().empty());
	CHECK(graph.Resources().front().imported);
}

