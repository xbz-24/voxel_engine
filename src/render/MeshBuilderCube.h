#pragma once

#include "GraphicsTypes.h"

#include <array>
#include <glm/glm.hpp>

namespace ve::rendering::detail
{
	/** Adds the six indexed quads of an axis-aligned cube. */
	template <typename Builder>
	Builder& AddCube(Builder& builder, glm::vec3 center, glm::vec3 size, ColorRgba color)
	{
		const glm::vec3 half_size = size * 0.5f;
		const glm::vec3 minimum = center - half_size;
		const glm::vec3 maximum = center + half_size;
		const std::array<glm::vec3, 8> corners{ {
			{ minimum.x, minimum.y, minimum.z }, { maximum.x, minimum.y, minimum.z },
			{ maximum.x, maximum.y, minimum.z }, { minimum.x, maximum.y, minimum.z },
			{ minimum.x, minimum.y, maximum.z }, { maximum.x, minimum.y, maximum.z },
			{ maximum.x, maximum.y, maximum.z }, { minimum.x, maximum.y, maximum.z } } };
		builder.AddQuad(corners[0], corners[1], corners[2], corners[3], color);
		builder.AddQuad(corners[4], corners[7], corners[6], corners[5], color);
		builder.AddQuad(corners[0], corners[4], corners[5], corners[1], color);
		builder.AddQuad(corners[1], corners[5], corners[6], corners[2], color);
		builder.AddQuad(corners[2], corners[6], corners[7], corners[3], color);
		builder.AddQuad(corners[3], corners[7], corners[4], corners[0], color);
		return builder;
	}
}
