#pragma once

#include <memory>

struct GLFWwindow;

namespace ve::engine
{
	struct GlfwWindowDeleter
	{
		void operator()(GLFWwindow* window) const noexcept;
	};

	using UniqueGlfwWindow = std::unique_ptr<GLFWwindow, GlfwWindowDeleter>;
}
