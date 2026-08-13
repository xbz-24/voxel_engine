#pragma once

#include "GraphicsTextureTypes.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <span>

namespace ve::rendering
{
	struct ColorRgba
	{
		float red = 1.0f;
		float green = 1.0f;
		float blue = 1.0f;
		float alpha = 1.0f;
	};

	struct ScreenRect
	{
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
	};

	struct ScreenTriangle
	{
		glm::vec2 first{ 0.0f };
		glm::vec2 second{ 0.0f };
		glm::vec2 third{ 0.0f };
	};

	struct Vertex3D
	{
		glm::vec3 position{ 0.0f };
		glm::vec3 normal{ 0.0f, 1.0f, 0.0f };
		glm::vec2 uv{ 0.0f };
		ColorRgba color{};
	};

	struct ChunkVertex
	{
		float x, y, z;
		float u, v;
		float r, g, b;
	};

	struct ChunkMeshBatch
	{
		TextureHandle texture = kInvalidTextureHandle;
		std::uint32_t first_vertex = 0;
		std::uint32_t vertex_count = 0;
	};

	/** Immutable mesh payload passed to a backend resource object for upload. */
	struct MeshDescription
	{
		std::span<const ChunkVertex> vertices;
		std::span<const ChunkMeshBatch> batches;
	};

	struct GraphicsAabb
	{
		glm::vec3 minimum{ 0.0f };
		glm::vec3 maximum{ 0.0f };

		[[nodiscard]] static GraphicsAabb Empty() noexcept;
		void Expand(const glm::vec3& point) noexcept;
		[[nodiscard]] bool Contains(const glm::vec3& point) const noexcept;
		[[nodiscard]] bool IsValid() const noexcept;
		[[nodiscard]] glm::vec3 Center() const noexcept;
		[[nodiscard]] glm::vec3 Size() const noexcept;
	};
}
