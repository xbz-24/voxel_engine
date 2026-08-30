#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <utility>

namespace
{
	voxel::EngineConfig InvalidWindowConfig(int width, int height)
	{
		return voxel::EngineConfig::Default()
			.WithWindow(voxel::Window("Invalid", width, height));
	}

	void CheckInertMovedFromEngine(voxel::Engine& engine)
	{
		CHECK(engine.Run() == -1);
		CHECK(!engine.Start());
		const voxel::EngineStartResult result = engine.StartDetailed();
		CHECK(result.failure == voxel::EngineStartFailure::RuntimeUnavailable);
		CHECK(result.message == "Engine instance was moved from");
		CHECK(!engine.Step());
		engine.RequestStop();
		engine.Shutdown();
		CHECK(!engine.IsRunning());
	}

	void CheckInvalidWidthEngine(voxel::Engine& engine)
	{
		const voxel::EngineStartResult result = engine.StartDetailed();
		CHECK(result.failure == voxel::EngineStartFailure::InvalidConfiguration);
		REQUIRE(result.issues.size() == 1);
		CHECK(result.issues.front() == "window.width must be greater than zero");
	}
}

TEST_CASE("public engine is safely inert after move construction")
{
	voxel::Engine source{ InvalidWindowConfig(0, 600) };
	voxel::Engine destination{ std::move(source) };

	CheckInertMovedFromEngine(source);
	CheckInvalidWidthEngine(destination);
}

TEST_CASE("public engine is safely inert after move assignment")
{
	voxel::Engine source{ InvalidWindowConfig(0, 600) };
	voxel::Engine destination{ InvalidWindowConfig(800, 0) };
	destination = std::move(source);

	CheckInertMovedFromEngine(source);
	CheckInvalidWidthEngine(destination);
}
