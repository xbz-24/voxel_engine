#pragma once

#include "GraphicsTypes.h"

#include <cstdint>
#include <string>
#include <variant>

namespace ve::rendering
{
	struct RenderSortKey
	{
		int layer = 0;
		std::uint64_t material = 0;
		float depth = 0.0f;
	};

	struct DrawTriangle2DCommand
	{
		ScreenTriangle triangle;
		ColorRgba color;
	};

	struct DrawRect2DCommand
	{
		ScreenRect rect;
		ColorRgba color;
		bool is_filled = true;
		float line_width = 1.0f;
	};

	struct DrawTexturedQuad2DCommand
	{
		TextureHandle texture = kInvalidTextureHandle;
		ScreenRect rect;
		ColorRgba tint;
	};

	struct DrawText2DCommand
	{
		std::string text;
		glm::vec2 origin{ 0.0f };
		float scale = 1.0f;
		ColorRgba color;
	};

	struct DrawCube3DCommand
	{
		glm::vec3 center{ 0.0f };
		glm::vec3 size{ 1.0f };
		ColorRgba color;
		bool is_filled = true;
		float line_width = 1.0f;
	};

	struct DrawInstancedMeshCommand
	{
		std::uint64_t mesh_id = 0;
		std::uint32_t instance_count = 0;
		glm::mat4 transform{ 1.0f };
		ColorRgba tint;
	};

	struct ScissorRectCommand
	{
		ScreenRect rect;
		bool enabled = true;
	};

	using RenderCommandPayload = std::variant<
		DrawTriangle2DCommand,
		DrawRect2DCommand,
		DrawTexturedQuad2DCommand,
		DrawText2DCommand,
		DrawCube3DCommand,
		DrawInstancedMeshCommand,
		ScissorRectCommand>;

	struct RenderCommand
	{
		RenderSortKey sort_key{};
		RenderCommandPayload payload;
	};
}
