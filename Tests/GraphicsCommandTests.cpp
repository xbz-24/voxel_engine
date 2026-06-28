#include <doctest/doctest.h>

#include "GraphicsFacade.h"
#include "GraphicsResourcePool.h"
#include "FrameGraph.h"
#include "MortonCode.h"
#include "RenderCommandSorter.h"

#include <utility>

namespace
{
	// TODO: Extend this executor to validate command payloads once text/textured primitives land.
	class RecordingRenderCommandExecutor final : public ve::rendering::RenderCommandExecutor
	{
	public:
		/** @param command_list Submitted commands inspected by this test double. */
		void Execute(const ve::rendering::RenderCommandList& command_list) override
		{
			executed_command_count = command_list.Count();
		}

		std::size_t executed_command_count = 0;
	};
}

TEST_CASE("resource pool rejects stale handles")
{
	ve::rendering::GraphicsResourcePool<int> resources;
	const ve::rendering::GraphicsResourceHandle first_handle = resources.Create(42);

	REQUIRE(resources.TryGet(first_handle) != nullptr);
	CHECK(*resources.TryGet(first_handle) == 42);
	CHECK(resources.Destroy(first_handle));
	CHECK(resources.TryGet(first_handle) == nullptr);
	CHECK(resources.Count() == 0U);
}

TEST_CASE("morton code roundtrips 3d coordinates")
{
	const ve::rendering::MortonCoordinate coordinate{ 17U, 9U, 5U };
	const std::uint64_t encoded = ve::rendering::EncodeMorton3D(coordinate);
	const ve::rendering::MortonCoordinate decoded = ve::rendering::DecodeMorton3D(encoded);

	CHECK(decoded.x == coordinate.x);
	CHECK(decoded.y == coordinate.y);
	CHECK(decoded.z == coordinate.z);
}

TEST_CASE("render command list sorts by submission key")
{
	ve::rendering::RenderCommandList commands;
	commands.DrawSolidRect({ 0.0f, 0.0f, 1.0f, 1.0f }, {}, { .layer = 2, .material = 2 });
	commands.DrawSolidRect({ 0.0f, 0.0f, 1.0f, 1.0f }, {}, { .layer = 1, .material = 9 });
	commands.DrawTriangle(
		{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f } }, {},
		{ .layer = 2, .material = 1 });

	ve::rendering::RenderCommandSorter::SortForSubmission(commands.MutableCommands());

	REQUIRE(commands.Count() == 3U);
	CHECK(commands.Commands()[0].sort_key.layer == 1);
	CHECK(commands.Commands()[1].sort_key.material == 1U);
	CHECK(commands.Commands()[2].sort_key.material == 2U);
}

TEST_CASE("graphics facade records and submits common primitives")
{
	RecordingRenderCommandExecutor executor;
	ve::rendering::GraphicsFacade graphics{ executor };

	graphics.BeginFrame();
	graphics.DrawTriangle({ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f } }, {});
	graphics.DrawRectangle({ 0.0f, 0.0f, 10.0f, 10.0f }, {});
	graphics.DrawCube({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, {});
	graphics.Submit();

	CHECK(graphics.PendingCommandCount() == 3U);
	CHECK(executor.executed_command_count == 3U);
}

TEST_CASE("frame graph records resource dependencies")
{
	// TODO: Add ordering/validation tests when FrameGraph starts sorting passes by declared resource dependencies.
	ve::rendering::FrameGraph graph;
	const ve::rendering::FrameGraphResourceHandle color = graph.DeclareResource("color");
	const ve::rendering::FrameGraphResourceHandle depth = graph.DeclareResource("depth");

	ve::rendering::FrameGraphPass pass{};
	pass.name = "geometry";
	pass.writes = { color, depth };
	graph.AddPass(std::move(pass));

	REQUIRE(graph.ResourceCount() == 2U);
	REQUIRE(graph.PassCount() == 1U);
	CHECK(graph.Passes().front().writes.size() == 2U);
	CHECK(graph.Passes().front().writes.front() == color);
}
