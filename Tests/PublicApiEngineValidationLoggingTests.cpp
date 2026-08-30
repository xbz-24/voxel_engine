#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <stdexcept>
#include <string>

TEST_CASE("invalid config logging contains reentrant lifecycle calls and sink failures")
{
	voxel::Engine* engine = nullptr;
	int callback_count = 0;
	int nested_run_result = 0;
	bool nested_start_result = true;
	std::string reported_message;
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow("Invalid", 0, 720)
		.OnLog([&](const std::string& message) {
			++callback_count;
			reported_message = message;
			nested_run_result = engine->Run();
			nested_start_result = engine->Start();
			engine->Shutdown();
			throw std::runtime_error("validation sink failure");
		});
	voxel::Engine subject{ invalid };
	engine = &subject;

	int run_result = 0;
	CHECK_NOTHROW(run_result = subject.Run());
	CHECK(run_result == -1);
	CHECK(callback_count == 1);
	CHECK(nested_run_result == -1);
	CHECK(!nested_start_result);
	CHECK(reported_message ==
		"Invalid EngineConfig: window.width must be greater than zero");

	voxel::EngineStartResult start_result;
	CHECK_NOTHROW(start_result = subject.StartDetailed());
	CHECK(callback_count == 2);
	CHECK(start_result.failure == voxel::EngineStartFailure::InvalidConfiguration);
	CHECK(start_result.message == "EngineConfig is invalid");
	REQUIRE(start_result.issues.size() == 1);
	CHECK(start_result.issues.front() == "window.width must be greater than zero");
}
