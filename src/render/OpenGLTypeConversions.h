#pragma once

#include "CoreTypes.h"

#include <GL/glew.h>

#include <cstddef>
#include <cstdint>

namespace ve::rendering
{
	template <typename Value>
	[[nodiscard]] constexpr GLint OpenGLInt(Value value) noexcept
	{
		return ve::core::NumericCast<GLint>(value);
	}

	template <typename Value>
	[[nodiscard]] constexpr GLsizei OpenGLCount(Value value) noexcept
	{
		return ve::core::NumericCast<GLsizei>(value);
	}

	template <typename Value>
	[[nodiscard]] constexpr GLsizeiptr OpenGLByteCount(Value value) noexcept
	{
		return ve::core::NumericCast<GLsizeiptr>(value);
	}

	[[nodiscard]] constexpr std::uint32_t OpenGLMeshIndex(std::size_t value) noexcept
	{
		return ve::core::ToU32(value);
	}

	[[nodiscard]] inline const void* OpenGLIndexByteOffset(std::uint32_t first_index) noexcept
	{
		const std::uintptr_t byte_offset =
			ve::core::NumericCast<std::uintptr_t>(first_index) * sizeof(std::uint32_t);
		return reinterpret_cast<const void*>(byte_offset);
	}
}
