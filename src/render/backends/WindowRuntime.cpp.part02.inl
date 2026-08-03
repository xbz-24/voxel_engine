ve::rendering::GraphicsApi ve::engine::Window::GraphicsApi() const noexcept
{
	return _graphicsApi;
}

std::vector<const char*> ve::engine::Window::RequiredVulkanInstanceExtensions() const
{
	std::uint32_t extension_count = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);
	if (extensions == nullptr) return {};
	return { extensions, extensions + extension_count };
}

void ve::engine::Window::SetCallbackUserData(void* userData)
{
	_callbackContext.userData = userData;
}

ve::engine::Window::CallbackContext* ve::engine::Window::GetCallbackContext(GLFWwindow* window)
{
	return static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
}

void* ve::engine::Window::GetCallbackUserData(GLFWwindow* window)
{
	CallbackContext* context = GetCallbackContext(window);
	return context != nullptr ? context->userData : nullptr;
}

void ve::engine::Window::RecordFramebufferResize(int width, int height)
{
	_width = width;
	_height = height;
	_eventQueue.push_back(WindowEvent{
		.kind = WindowEvent::Kind::FramebufferResized,
		.framebuffer_resized = WindowFramebufferResizeEvent{ width, height }
	});
}

void ve::engine::Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) noexcept
{
	CallbackContext* context = GetCallbackContext(window);
	if (context && context->window)
	{
		context->window->RecordFramebufferResize(width, height);
	}
}
