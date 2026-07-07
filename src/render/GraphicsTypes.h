#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <span>

namespace ve::rendering
{
	/** Backend-neutral texture identifier. Native API ids stay behind backend upload/bind code. */
	struct TextureHandle
	{
		std::uint32_t value = 0;

		[[nodiscard]] constexpr bool IsValid() const noexcept { return value != 0; }
		[[nodiscard]] explicit constexpr operator bool() const noexcept { return IsValid(); }
	};

	[[nodiscard]] constexpr bool operator==(TextureHandle left, TextureHandle right) noexcept { return left.value == right.value; }
	[[nodiscard]] constexpr bool operator!=(TextureHandle left, TextureHandle right) noexcept { return !(left == right); }
	[[nodiscard]] constexpr bool operator<(TextureHandle left, TextureHandle right) noexcept { return left.value < right.value; }
	[[nodiscard]] constexpr bool operator==(TextureHandle left, std::uint32_t right) noexcept { return left.value == right; }
	[[nodiscard]] constexpr bool operator==(std::uint32_t left, TextureHandle right) noexcept { return left == right.value; }

	inline constexpr TextureHandle kInvalidTextureHandle{};

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
