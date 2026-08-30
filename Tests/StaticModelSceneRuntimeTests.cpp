#include <doctest/doctest.h>

#include "StaticModelSceneConfiguration.h"
#include "StaticModelSceneRenderTestDoubles.h"
#include "StaticModelSceneRuntime.h"
#include "StaticModelSceneRuntimeTestSupport.h"

#include <filesystem>
#include <memory>

TEST_CASE("static model runtime imports uploads draws and releases one OBJ mesh")
{
	ve::tests::TemporaryStaticObj model_file;
	auto state = std::make_shared<ve::tests::StaticMeshResourceState>();
	ve::tests::RecordingStaticMeshBackend backend{ state };
	ve::engine::StaticModelSceneRuntime runtime;
	const ve::engine::StaticModelSceneConfiguration configuration{ model_file.Path(), true };

	const ve::engine::EngineStartupResult result = runtime.Initialize(configuration, backend);
	REQUIRE(result);
	CHECK(state->creation_attempts == 1);
	CHECK(state->upload_count == 1);
	CHECK(state->vertices.size() == 3);
	CHECK(state->indices.size() == 3);
	runtime.Draw();
	CHECK(state->draw_count == 1);
	CHECK(state->model_matrix[0][0] == 1.0f);
	CHECK(state->model_matrix[3][3] == 1.0f);
	runtime.Shutdown();
	runtime.Shutdown();
	CHECK(state->release_count == 1);
}

TEST_CASE("static model runtime preserves hidden scene visibility")
{
	ve::tests::TemporaryStaticObj model_file;
	auto state = std::make_shared<ve::tests::StaticMeshResourceState>();
	ve::tests::RecordingStaticMeshBackend backend{ state };
	{
		ve::engine::StaticModelSceneRuntime runtime;
		const ve::engine::StaticModelSceneConfiguration configuration{
			model_file.Path(), false };
		REQUIRE(runtime.Initialize(configuration, backend));
		runtime.Draw();
		CHECK(state->draw_count == 0);
	}
	CHECK(state->release_count == 1);
}

TEST_CASE("static model runtime reports backend resource creation failure")
{
	ve::tests::TemporaryStaticObj model_file;
	auto state = std::make_shared<ve::tests::StaticMeshResourceState>();
	ve::tests::RecordingStaticMeshBackend backend{ state };
	backend.reject_creation = true;
	ve::engine::StaticModelSceneRuntime runtime;
	const auto result = runtime.Initialize(
		ve::engine::StaticModelSceneConfiguration{ model_file.Path(), true }, backend);

	CHECK_FALSE(result);
	CHECK(result.failure ==
		ve::engine::EngineStartupFailure::StaticModelResourceCreationFailed);
	CHECK(result.message == "Render backend could not create the static model mesh resource");
	CHECK(state->creation_attempts == 1);
	CHECK(state->release_count == 0);
}

TEST_CASE("static model runtime reports importer failure before resource creation")
{
	ve::tests::TemporaryStaticObj model_file;
	auto state = std::make_shared<ve::tests::StaticMeshResourceState>();
	ve::tests::RecordingStaticMeshBackend backend{ state };
	ve::engine::StaticModelSceneRuntime runtime;
	const auto result = runtime.Initialize(ve::engine::StaticModelSceneConfiguration{
		model_file.Path().parent_path() / "missing.obj", true }, backend);

	CHECK_FALSE(result);
	CHECK(result.failure == ve::engine::EngineStartupFailure::StaticModelImportFailed);
	CHECK(state->creation_attempts == 0);
}
