#pragma once

#include "GraphicsMesh.h"

#include <array>
#include <concepts>
#include <glm/geometric.hpp>
#include <utility>

namespace ve::rendering
{
	template <typename VertexAllocator = std::allocator<Vertex3D>, typename IndexAllocator = std::allocator<std::uint32_t>>
	class BasicMeshBuilder
	{
	public:
		using Mesh = BasicIndexedMesh<VertexAllocator, IndexAllocator>;

		/** Creates a builder with default-constructed vector allocators. */
		BasicMeshBuilder()
			requires std::default_initializable<VertexAllocator> && std::default_initializable<IndexAllocator>
			= default;

		/** @param vertex_allocator Allocator used by vertex storage. @param index_allocator Allocator used by index storage. */
		explicit BasicMeshBuilder(const VertexAllocator& vertex_allocator, const IndexAllocator& index_allocator)
			: mesh_{ PrimitiveTopology::Triangles, typename Mesh::VertexList{ vertex_allocator },
				typename Mesh::IndexList{ index_allocator } }
		{
		}

		/** @param vertex_count Vertex capacity. @param index_count Index capacity. @return This builder for chaining. */
		BasicMeshBuilder& Reserve(std::size_t vertex_count, std::size_t index_count)
		{
			mesh_.vertices.reserve(vertex_count);
			mesh_.indices.reserve(index_count);
			return *this;
		}

		/** @param first First triangle point. @param second Second point. @param third Third point. @param color Vertex color. @return This builder. */
		BasicMeshBuilder& AddTriangle(glm::vec3 first, glm::vec3 second, glm::vec3 third, ColorRgba color = {})
		{
			const glm::vec3 normal = FaceNormal(first, second, third);
			const std::uint32_t first_index = AddVertex(first, normal, color);
			const std::uint32_t second_index = AddVertex(second, normal, color);
			const std::uint32_t third_index = AddVertex(third, normal, color);
			AddIndexedTriangle(first_index, second_index, third_index);
			return *this;
		}

		/** @param first First quad corner. @param second Second corner. @param third Third corner. @param fourth Fourth corner. @param color Vertex color. @return This builder. */
		BasicMeshBuilder& AddQuad(glm::vec3 first, glm::vec3 second, glm::vec3 third, glm::vec3 fourth, ColorRgba color = {})
		{
			const glm::vec3 normal = FaceNormal(first, second, third);
			const std::uint32_t first_index = AddVertex(first, normal, color);
			const std::uint32_t second_index = AddVertex(second, normal, color);
			const std::uint32_t third_index = AddVertex(third, normal, color);
			const std::uint32_t fourth_index = AddVertex(fourth, normal, color);
			AddIndexedTriangle(first_index, second_index, third_index);
			AddIndexedTriangle(first_index, third_index, fourth_index);
			return *this;
		}

		/** @param center Cube center. @param size Cube size by axis. @param color Vertex color. @return This builder. */
		BasicMeshBuilder& AddCube(glm::vec3 center, glm::vec3 size, ColorRgba color = {})
		{
			const glm::vec3 half_size = size * 0.5f;
			const glm::vec3 minimum = center - half_size;
			const glm::vec3 maximum = center + half_size;
			const std::array<glm::vec3, 8> corners{ {
				{ minimum.x, minimum.y, minimum.z }, { maximum.x, minimum.y, minimum.z },
				{ maximum.x, maximum.y, minimum.z }, { minimum.x, maximum.y, minimum.z },
				{ minimum.x, minimum.y, maximum.z }, { maximum.x, minimum.y, maximum.z },
				{ maximum.x, maximum.y, maximum.z }, { minimum.x, maximum.y, maximum.z } } };
			AddQuad(corners[0], corners[1], corners[2], corners[3], color);
			AddQuad(corners[4], corners[7], corners[6], corners[5], color);
			AddQuad(corners[0], corners[4], corners[5], corners[1], color);
			AddQuad(corners[1], corners[5], corners[6], corners[2], color);
			AddQuad(corners[2], corners[6], corners[7], corners[3], color);
			AddQuad(corners[3], corners[7], corners[4], corners[0], color);
			return *this;
		}

		/** @return Mesh data moved out of the builder. */
		[[nodiscard]] Mesh Build() && noexcept { return std::move(mesh_); }

	private:
		/** @param position Vertex position. @param normal Vertex normal. @param color Vertex color. @return New vertex index. */
		std::uint32_t AddVertex(glm::vec3 position, glm::vec3 normal, ColorRgba color)
		{
			mesh_.vertices.push_back(Vertex3D{ position, normal, glm::vec2{ 0.0f }, color });
			return static_cast<std::uint32_t>(mesh_.vertices.size() - 1U);
		}

		/** @param first First index. @param second Second index. @param third Third index. */
		void AddIndexedTriangle(std::uint32_t first, std::uint32_t second, std::uint32_t third)
		{
			mesh_.indices.push_back(first);
			mesh_.indices.push_back(second);
			mesh_.indices.push_back(third);
		}

		/** @param first First triangle point. @param second Second point. @param third Third point. @return Unit face normal. */
		[[nodiscard]] static glm::vec3 FaceNormal(glm::vec3 first, glm::vec3 second, glm::vec3 third)
		{
			const glm::vec3 cross_product = glm::cross(second - first, third - first);
			return glm::length(cross_product) > 0.0f ? glm::normalize(cross_product) : glm::vec3{ 0.0f, 1.0f, 0.0f };
		}

		Mesh mesh_;
	};

	using MeshBuilder = BasicMeshBuilder<>;
}
