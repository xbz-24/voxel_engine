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
