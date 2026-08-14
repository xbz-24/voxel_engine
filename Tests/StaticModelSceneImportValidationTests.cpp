#include <doctest/doctest.h>

#include "ImportedModel.h"
#include "StaticModelSceneImport.h"

#include <string>
#include <utility>

namespace
{
	ve::assets::ImportedModel ValidStaticModel()
	{
		ve::assets::ImportedModel model;
		model.has_root_node = true;
		ve::assets::ImportedMesh mesh;
		mesh.vertices.resize(3);
		mesh.indices = { 0, 1, 2 };
		model.meshes.push_back(std::move(mesh));
		return model;
	}

	void CheckValidationFailure(
		const ve::assets::ImportedModel& model, const std::string& message)
	{
		ve::engine::StaticModelSceneMeshData mesh_data;
		const auto result = ve::engine::ConvertStaticModelSceneMesh(model, mesh_data);
		CHECK_FALSE(result);
		CHECK(result.failure == ve::engine::EngineStartupFailure::StaticModelValidationFailed);
		CHECK(result.message == message);
	}
}

TEST_CASE("static model conversion rejects non-static scene metadata")
{
	ve::assets::ImportedModel model = ValidStaticModel();
	SUBCASE("camera")
	{
		model.source_camera_count = 1;
		CheckValidationFailure(model, "Static model scene must not contain cameras");
	}
	SUBCASE("light")
	{
		model.source_light_count = 1;
		CheckValidationFailure(model, "Static model scene must not contain lights");
	}
	SUBCASE("animation")
	{
		model.source_animation_count = 1;
		CheckValidationFailure(model, "Static model scene must not contain animations");
	}
	SUBCASE("nested node transform")
	{
		model.root_node.children.push_back(ve::assets::ImportedNode{});
		model.root_node.children.front().transform[3][0] = 2.0f;
		CheckValidationFailure(model, "Static model scene nodes must use identity transforms");
	}
	SUBCASE("skinning joints")
	{
		model.meshes.front().joints.push_back(ve::assets::ImportedJoint{});
		CheckValidationFailure(model,
			"Static model scene mesh must not contain skinning joints");
	}
}

TEST_CASE("static model conversion rejects invalid topology")
{
	ve::assets::ImportedModel model = ValidStaticModel();
	SUBCASE("mesh count")
	{
		model.meshes.push_back(ve::assets::ImportedMesh{});
		CheckValidationFailure(model, "Static model scene must contain exactly one mesh");
	}
	SUBCASE("incomplete triangle")
	{
		model.meshes.front().indices = { 0, 1 };
		CheckValidationFailure(model,
			"Static model scene mesh indices must describe complete triangles");
	}
	SUBCASE("out of range")
	{
		model.meshes.front().indices = { 0, 1, 4 };
		CheckValidationFailure(model,
			"Static model scene mesh contains an out-of-range index");
	}
}
