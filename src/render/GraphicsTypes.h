#pragma once

#include <glm/glm.hpp>

#include <cstdint>

namespace ve::rendering
{
	using TextureHandle = std::uint32_t;
	inline constexpr TextureHandle kInvalidTextureHandle = 0;

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

	struct GraphicsAabb
	{
		glm::vec3 minimum{ 0.0f };
		glm::vec3 maximum{ 0.0f };

		/** @return Invalid empty bounds ready to be expanded by vertices. */
		[[nodiscard]] static GraphicsAabb Empty() noexcept;

		/** @param point Point that should fit inside this bounds. */
		void Expand(const glm::vec3& point) noexcept;

		/** @param point Point to test. @return True when the point is inside the box. */
		[[nodiscard]] bool Contains(const glm::vec3& point) const noexcept;

		/** @return True when minimum and maximum describe a valid box. */
		[[nodiscard]] bool IsValid() const noexcept;

		/** @return Center point of the bounds. */
		[[nodiscard]] glm::vec3 Center() const noexcept;

		/** @return Size of the bounds along each axis. */
		[[nodiscard]] glm::vec3 Size() const noexcept;
	};
}
