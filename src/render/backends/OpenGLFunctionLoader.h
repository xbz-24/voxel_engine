#pragma once

namespace ve::rendering
{
	/** Loads modern OpenGL entry points for the current context. */
	[[nodiscard]] bool InitializeOpenGLFunctionLoader() noexcept;
}
