#include <doctest/doctest.h>

#include "StaticModelSceneConfiguration.h"
#include "StaticModelSceneRenderTestDoubles.h"
#include "StaticModelSceneRuntime.h"
#include "StaticModelSceneRuntimeTestSupport.h"

#include <glm/vec4.hpp>

#include <limits>
#include <memory>

TEST_CASE("static model runtime draws a translated root and resets on reinitialize")
{
	ve::tests::TemporaryStaticObj model_file;
	auto state = std::make_shared<ve::tests::StaticMeshResourceState>();
	ve::tests::RecordingStaticMeshBackend backend{ state };
	ve::engine::StaticModelSceneRuntime runtime;
	const ve::engine::StaticModelSceneConfiguration translated{
		model_file.Path(), true, { 2.0f, -3.0f, 4.0f } };
	REQUIRE(runtime.Initialize(translated, backend));

	runtime.Draw();
	const glm::vec4 origin{ 0.0f, 0.0f, 0.0f, 1.0f };
	const glm::vec4 moved_origin = state->model_matrix * origin;
	CHECK(moved_origin.x == 2.0f);
	CHECK(moved_origin.y == -3.0f);
	CHECK(moved_origin.z == 4.0f);
	CHECK(moved_origin.w == 1.0f);

	REQUIRE(runtime.Initialize(
		ve::engine::StaticModelSceneConfiguration{ model_file.Path(), true }, backend));
	runtime.Draw();
	const glm::vec4 reset_origin = state->model_matrix * origin;
	CHECK(reset_origin.x == 0.0f);
	CHECK(reset_origin.y == 0.0f);
	CHECK(reset_origin.z == 0.0f);
	CHECK(state->draw_count == 2);
}

TEST_CASE("static model runtime rejects non-finite translation before import and upload")
{
	auto state = std::make_shared<ve::tests::StaticMeshResourceState>();
	ve::tests::RecordingStaticMeshBackend backend{ state };
	ve::engine::StaticModelSceneRuntime runtime;
	const ve::engine::StaticModelSceneConfiguration invalid{
		"missing.obj", true, { std::numeric_limits<float>::infinity(), 0.0f, 0.0f } };

	const ve::engine::EngineStartupResult result = runtime.Initialize(invalid, backend);
	CHECK_FALSE(result);
	CHECK(result.failure == ve::engine::EngineStartupFailure::StaticModelValidationFailed);
	CHECK(result.message == "Static model scene root translation must be finite");
	CHECK(state->creation_attempts == 0);
}
