#include <doctest/doctest.h>

#include "ImportedModel.h"
#include "StaticModelSceneImport.h"

#include <cstdint>
#include <utility>
#include <vector>

TEST_CASE("static model conversion preserves portable vertex data")
{
	ve::assets::ImportedModel model;
	ve::assets::ImportedMesh source_mesh;
	source_mesh.vertices.resize(3);
	source_mesh.indices = { 0, 1, 2 };
	auto& vertex = source_mesh.vertices.front();
	vertex.position = { 2.0f, 3.0f, 4.0f };
	vertex.normal = { 0.0f, 0.0f, 1.0f };
	vertex.texture_coordinates = { 0.25f, 0.75f };
	vertex.color = { 0.1f, 0.2f, 0.3f, 0.4f };
	model.meshes.push_back(std::move(source_mesh));
	ve::engine::StaticModelSceneMeshData mesh_data;

	REQUIRE(ve::engine::ConvertStaticModelSceneMesh(model, mesh_data));
	REQUIRE(mesh_data.vertices.size() == 3);
	CHECK(mesh_data.vertices.front().position.x == 2.0f);
	CHECK(mesh_data.vertices.front().normal.z == 1.0f);
	CHECK(mesh_data.vertices.front().uv.y == 0.75f);
	CHECK(mesh_data.vertices.front().color.alpha == 0.4f);
	CHECK((mesh_data.indices == std::vector<std::uint32_t>{ 0, 1, 2 }));
}
