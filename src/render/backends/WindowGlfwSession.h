#pragma once

namespace ve::engine::detail
{
	/** Acquires the process GLFW session on its required host thread. */
	[[nodiscard]] bool AcquireGlfwSession() noexcept;

	/** Releases one acquisition and terminates GLFW after the last window. */
	void ReleaseGlfwSession() noexcept;
}
