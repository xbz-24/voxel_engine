#include <doctest/doctest.h>

#include "FrameGraph.h"

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

TEST_CASE("frame graph executes producers before consumers")
{
	ve::rendering::FrameGraph graph;
	const ve::rendering::FrameGraphResourceHandle color = graph.DeclareResource("color");
	std::vector<int> execution_order;

	ve::rendering::FrameGraphPass consumer{};
	consumer.name = "tonemap";
	consumer.reads = { color };
	consumer.execute = [&execution_order](ve::rendering::FrameGraphContext&) {
		execution_order.push_back(2);
	};
	graph.AddPass(std::move(consumer));

	ve::rendering::FrameGraphPass producer{};
	producer.name = "geometry";
	producer.writes = { color };
	producer.execute = [&execution_order](ve::rendering::FrameGraphContext&) {
		execution_order.push_back(1);
	};
	graph.AddPass(std::move(producer));

	ve::rendering::FrameGraphContext context{};
	graph.Execute(context);

	CHECK(execution_order == std::vector<int>{ 1, 2 });
	CHECK(context.frame_index == 1U);
}

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
