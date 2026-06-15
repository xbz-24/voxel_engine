#pragma once

#include "GraphicsTypes.h"

#include <cstdint>
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

	struct DrawCube3DCommand
	{
		glm::vec3 center{ 0.0f };
		glm::vec3 size{ 1.0f };
		ColorRgba color;
		bool is_filled = true;
		float line_width = 1.0f;
	};

	using RenderCommandPayload = std::variant<DrawTriangle2DCommand, DrawRect2DCommand, DrawCube3DCommand>;

	struct RenderCommand
	{
		RenderSortKey sort_key{};
		RenderCommandPayload payload;
	};
}
