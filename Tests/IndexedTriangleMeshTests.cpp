#include <doctest/doctest.h>

#include "DirectX12Backend.h"
#include "IndexedTriangleMesh.h"
#include "OpenGLCompatibilityBackend.h"

#include <array>
#include <cstdint>

TEST_CASE("indexed triangle mesh description validates triangle indices")
{
	const std::array<ve::rendering::Vertex3D, 3> vertices{};
	const std::array<std::uint32_t, 3> triangle{{ 0U, 1U, 2U }};
	const std::array<std::uint32_t, 2> incomplete{{ 0U, 1U }};
	const std::array<std::uint32_t, 3> out_of_range{{ 0U, 1U, 3U }};

	CHECK(ve::rendering::IndexedTriangleMeshDescription{ vertices, triangle }.IsValid());
	CHECK_FALSE(ve::rendering::IndexedTriangleMeshDescription{ vertices, incomplete }.IsValid());
	CHECK_FALSE(ve::rendering::IndexedTriangleMeshDescription{ vertices, out_of_range }.IsValid());
	CHECK_FALSE(ve::rendering::IndexedTriangleMeshDescription{ {}, triangle }.IsValid());
}

TEST_CASE("indexed triangle mesh factories report unsupported and invalid uploads")
{
	const ve::rendering::IndexedTriangleMeshDescription empty{};
	const ve::rendering::DirectX12Backend unsupported;
	const ve::rendering::OpenGLCompatibilityBackend opengl;

	CHECK(unsupported.CreateIndexedTriangleMesh(empty) == nullptr);
	CHECK(opengl.CreateIndexedTriangleMesh(empty) == nullptr);
}
