TEST_CASE("public config validation reports invalid startup values")
{
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow("Invalid", 0, -1)
		.WithWorldSizeChunks(0)
		.WithRenderDistanceChunks(-1)
		.UseDirectX12()
		.WithAssets(voxel::AssetCatalog{}
			.Texture("duplicate", "a.png")
			.Texture("duplicate", "b.png")
			.Model("", ""));

	const std::vector<std::string> issues = invalid.Validate();

	CHECK(!invalid.IsValid());
	CHECK(issues.size() == 9);
	CHECK(std::find(issues.begin(), issues.end(),
		"AssetCatalog runtime loading is not implemented") != issues.end());
}

TEST_CASE("public engine rejects invalid config before runtime startup")
{
	std::vector<std::string> logs;
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow("Invalid", 0, 720)
		.OnLog([&logs](const std::string& line) {
			logs.push_back(line);
		});

	voxel::Engine engine{ invalid };

	CHECK(engine.Run() == -1);
	REQUIRE(!logs.empty());
	CHECK(logs.front() == "Invalid EngineConfig: window.width must be greater than zero");
}

TEST_CASE("public config validation rejects invalid display settings")
{
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow(voxel::Window("Invalid display")
			.UseMonitor(-1)
			.WithRefreshRate(-60));

	const std::vector<std::string> issues = invalid.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"window.monitor_index must be zero or greater") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"window.refresh_rate_hertz must be zero or greater") != issues.end());
}

TEST_CASE("public asset catalog validation rejects invalid search roots")
{
	voxel::AssetCatalog assets{};
	assets.SearchRoot("");

	const std::vector<std::string> issues = assets.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"asset search root must not be empty") != issues.end());
}

TEST_CASE("public engine config accepts runtime-bound asset search roots")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithAssets(voxel::AssetCatalog{}.SearchRoot("assets"));

	CHECK(config.IsValid());
}

TEST_CASE("public asset catalog validates source policies")
{
	voxel::AssetCatalog assets{};
	assets.Texture("empty-embedded", voxel::AssetSource::Embedded({}))
		.Model("bad-archive", voxel::AssetSource::Archive("", "crate.obj"))
		.Sound("bad-hot-reload", voxel::AssetSource::Embedded({ 1U }).EnableHotReload());

	const std::vector<std::string> issues = assets.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"texture embedded asset data must not be empty") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"model archive path must not be empty") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"sound asset hot reload requires a filesystem path") != issues.end());
}

TEST_CASE("public material library validation reports authoring range errors")
