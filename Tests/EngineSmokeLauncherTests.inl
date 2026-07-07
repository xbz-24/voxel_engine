TEST_CASE("command line launcher defaults to the demo suite")
{
	const ve::launcher::CommandLineArguments arguments{ "voxel_engine" };

	const ve::launcher::LaunchParseResult parsed =
		ve::launcher::ParseLaunchArguments(arguments);

	CHECK(parsed.ok);
	CHECK(parsed.options.launch_demo_suite);
	CHECK(parsed.options.selected_demo_name == "desert");
	CHECK(parsed.options.smoke_frame_limit == 0);
}

TEST_CASE("command line launcher parses single demo smoke runs")
{
	const ve::launcher::CommandLineArguments arguments{ "voxel_engine", "--demo", "aqua", "--smoke-frames", "3" };

	const ve::launcher::LaunchParseResult parsed =
		ve::launcher::ParseLaunchArguments(arguments);

	CHECK(parsed.ok);
	CHECK(!parsed.options.launch_demo_suite);
	CHECK(parsed.options.selected_demo_name == "aqua");
	CHECK(parsed.options.smoke_frame_limit == 3);
}

TEST_CASE("command line launcher parses demo suite aliases")
{
	const ve::launcher::CommandLineArguments arguments{ "voxel_engine", "--demo", "all-demos", "--smoke-frames", "2" };

	const ve::launcher::LaunchParseResult parsed =
		ve::launcher::ParseLaunchArguments(arguments);

	CHECK(parsed.ok);
	CHECK(parsed.options.launch_demo_suite);
	CHECK(parsed.options.selected_demo_name == "all-demos");
	CHECK(parsed.options.smoke_frame_limit == 2);
}

TEST_CASE("command line launcher rejects invalid arguments")
{
	const ve::launcher::CommandLineArguments arguments{ "voxel_engine", "--smoke-frames", "zero" };

	const ve::launcher::LaunchParseResult parsed =
		ve::launcher::ParseLaunchArguments(arguments);

	CHECK(!parsed.ok);
}

TEST_CASE("command line launcher rejects missing option values")
{
	const ve::launcher::CommandLineArguments arguments{ "voxel_engine", "--demo" };

	const ve::launcher::LaunchParseResult parsed =
		ve::launcher::ParseLaunchArguments(arguments);

	CHECK(!parsed.ok);
}
