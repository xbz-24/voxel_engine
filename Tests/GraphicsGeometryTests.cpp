#include <doctest/doctest.h>

#include "GeometryFactory.h"
#include "MeshAlgorithms.h"
#include "MeshBuilder.h"

#include <array>
#include <memory_resource>
#include <utility>

TEST_CASE("geometry factory builds indexed cube mesh")
{
	const ve::rendering::IndexedMesh cube = ve::rendering::GeometryFactory::Cube(
		{ 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
	const ve::rendering::GraphicsAabb bounds = cube.Bounds();

	CHECK(cube.vertices.size() == 24U);
	CHECK(cube.indices.size() == 36U);
	CHECK(cube.TriangleCount() == 12U);
	CHECK(bounds.Contains({ 1.0f, 1.0f, 1.0f }));
	CHECK(bounds.Contains({ -1.0f, -1.0f, -1.0f }));
}

TEST_CASE("mesh builder supports polymorphic allocators")
{
	std::array<std::byte, 4096> storage{};
	std::pmr::monotonic_buffer_resource memory_resource{ storage.data(), storage.size() };
	using VertexAllocator = std::pmr::polymorphic_allocator<ve::rendering::Vertex3D>;
	using IndexAllocator = std::pmr::polymorphic_allocator<std::uint32_t>;

	ve::rendering::BasicMeshBuilder<VertexAllocator, IndexAllocator> builder{
		VertexAllocator{ &memory_resource }, IndexAllocator{ &memory_resource } };
	builder.Reserve(3U, 3U).AddTriangle(
		{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	const auto triangle = std::move(builder).Build();

	CHECK(triangle.vertices.size() == 3U);
	CHECK(triangle.indices.size() == 3U);
}

TEST_CASE("mesh algorithms remove degenerate geometry and rebuild normals")
{
	ve::rendering::MeshBuilder builder;
	builder.AddTriangle({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	builder.AddTriangle({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	ve::rendering::IndexedMesh mesh = std::move(builder).Build();

	const std::size_t removed_count = ve::rendering::RemoveDegenerateTriangles(mesh);
	ve::rendering::RecalculateSmoothNormals(mesh);

	CHECK(removed_count == 1U);
	CHECK(mesh.TriangleCount() == 1U);
	CHECK(mesh.vertices[0].normal.z == doctest::Approx(1.0f));
}
