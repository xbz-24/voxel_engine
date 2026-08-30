#include <doctest/doctest.h>

#include "AmbientOcclusionPass.h"
#include "FrameGraph.h"
#include "TemporalAntiAliasingPass.h"

#include <string>
#include <utility>
#include <vector>

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
	ve::rendering::FrameGraphExecutionStats stats{};
	context.stats = &stats;
	graph.Execute(context);

	CHECK(execution_order == std::vector<int>{ 1, 2 });
	CHECK(context.frame_index == 1U);
	CHECK(stats.executed_pass_count == 2U);
}

