#include "WindowGlfwSession.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ve::engine::detail
{
	namespace
	{
		unsigned int active_session_count = 0;
	}

	bool AcquireGlfwSession() noexcept
	{
		if (active_session_count == 0 && glfwInit() != GLFW_TRUE) return false;
		++active_session_count;
		return true;
	}

	void ReleaseGlfwSession() noexcept
	{
		if (active_session_count == 0) return;
		--active_session_count;
		if (active_session_count == 0) glfwTerminate();
	}
}
