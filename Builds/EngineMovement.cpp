#include "Engine.h"

#include <algorithm>

namespace
{
	/**
	 * Projects a direction onto the ground plane and normalizes it.
	 */
	glm::vec3 Horizontal(glm::vec3 direction)
	{
		direction.y = 0.0f;
		return glm::length(direction) > 0.0f ? glm::normalize(direction) : direction;
	}

	/**
	 * Moves the camera when a key is pressed.
	 */
	void MoveWhenPressed(GLFWwindow* window, int key, Camera& camera, const glm::vec3& direction, float amount)
	{
		if (glfwGetKey(window, key) == GLFW_PRESS)
		{
			camera.Move(direction, amount);
		}
	}
}

void Engine::ProcessInput(Window& window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameTimeDeltaSeconds)
{
	const bool wasMenuOpen = _isSettingsMenuOpen;
	ProcessSettingsMenuInput(window);
	if (wasMenuOpen || _isSettingsMenuOpen)
	{
		return;
	}

	handlePlayerMovementAndWindowInput(window.GetNativeWindow(), world, blockRegistry, camera, frameTimeDeltaSeconds);
}

void Engine::handlePlayerMovementAndWindowInput(GLFWwindow* window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds)
{
	const bool flyTogglePressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
	if (flyTogglePressed && !_wasFlyTogglePressed)
	{
		_isFlying = !_isFlying;
		_verticalVelocity = 0.0f;
	}
	_wasFlyTogglePressed = flyTogglePressed;

	const bool decreasePressed = glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS;
	if (decreasePressed && !_wasRenderDistanceDecreasePressed)
	{
		_renderDistanceChunks = std::max(1, _renderDistanceChunks - 1);
	}
	_wasRenderDistanceDecreasePressed = decreasePressed;

	const bool increasePressed = glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS;
	if (increasePressed && !_wasRenderDistanceIncreasePressed)
	{
		_renderDistanceChunks = std::min(6, _renderDistanceChunks + 1);
	}
	_wasRenderDistanceIncreasePressed = increasePressed;

	const float speed = 5.0f * static_cast<float>(frameDeltaTimeSeconds);
	const glm::vec3 forward = Horizontal(camera.GetForward());
	const glm::vec3 right = Horizontal(camera.GetRight());
	MoveWhenPressed(window, GLFW_KEY_W, camera, forward, speed);
	MoveWhenPressed(window, GLFW_KEY_S, camera, forward, -speed);
	MoveWhenPressed(window, GLFW_KEY_A, camera, right, -speed);
	MoveWhenPressed(window, GLFW_KEY_D, camera, right, speed);

	if (_isFlying)
	{
		MoveWhenPressed(window, GLFW_KEY_SPACE, camera, glm::vec3(0.0f, 1.0f, 0.0f), speed);
		MoveWhenPressed(window, GLFW_KEY_LEFT_SHIFT, camera, glm::vec3(0.0f, -1.0f, 0.0f), speed);
	}
	else if (_isGrounded && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		_verticalVelocity = 7.0f;
	}
	ApplyPlayerPhysics(world, blockRegistry, camera, frameDeltaTimeSeconds);
}
