	void VulkanInputController::Update(GLFWwindow* window)
	{
		previous_state_ = current_state_;

		current_state_.set(static_cast<size_t>(Action::LeftClick), glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		current_state_.set(static_cast<size_t>(Action::F1), glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS);
		current_state_.set(static_cast<size_t>(Action::F2), glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS);
	}

	bool VulkanInputController::IsDown(Action action) const
	{
		return current_state_[static_cast<size_t>(action)];
	}

	bool VulkanInputController::IsJustPressed(Action action) const
	{
		return current_state_[static_cast<size_t>(action)] && !previous_state_[static_cast<size_t>(action)];
	}
}
