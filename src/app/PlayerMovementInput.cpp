#include "PlayerMovementInput.h"

#include "Camera.h"
#include "Input.h"

#include <glm/glm.hpp>

namespace
{
	/// Projects a direction onto the ground plane and normalizes it.
	glm::vec3 Horizontal(glm::vec3 direction)
	{
		direction.y = 0.0f;
		return glm::length(direction) > 0.0f ? glm::normalize(direction) : direction;
	}

	/// Moves the camera only when the intent axis is active.
	void MoveWhenRequested(bool shouldMove, Camera& camera, const glm::vec3& direction, float amount)
	{
		if (shouldMove)
		{
			camera.Move(direction, amount);
		}
	}
}

namespace ve::gameplay
{
	/// Reads movement keys into an engine-owned intent structure.
	PlayerMoveIntent ReadPlayerMoveIntent(const ve::input::InputSnapshot& input) noexcept
	{
		return {
			ve::input::IsPressed(input, ve::input::Key::W),
			ve::input::IsPressed(input, ve::input::Key::S),
			ve::input::IsPressed(input, ve::input::Key::A),
			ve::input::IsPressed(input, ve::input::Key::D),
			ve::input::IsPressed(input, ve::input::Key::Space),
			ve::input::IsPressed(input, ve::input::Key::LeftShift)
		};
	}

	/// Applies horizontal movement to the camera from an intent.
	void ApplyPlanarMovement(const PlayerMoveIntent& intent, Camera& camera, float amount)
	{
		const glm::vec3 forward = Horizontal(camera.GetForward());
		const glm::vec3 right = Horizontal(camera.GetRight());
		MoveWhenRequested(intent.moveForward, camera, forward, amount);
		MoveWhenRequested(intent.moveBackward, camera, forward, -amount);
		MoveWhenRequested(intent.strafeLeft, camera, right, -amount);
		MoveWhenRequested(intent.strafeRight, camera, right, amount);
	}

	/// Applies vertical flying movement to the camera from an intent.
	void ApplyFlyingMovement(const PlayerMoveIntent& intent, Camera& camera, float amount)
	{
		MoveWhenRequested(intent.ascend, camera, glm::vec3(0.0f, 1.0f, 0.0f), amount);
		MoveWhenRequested(intent.descend, camera, glm::vec3(0.0f, -1.0f, 0.0f), amount);
	}

	/// Checks whether the intent requests a jump.
	bool WantsJump(const PlayerMoveIntent& intent) noexcept
	{
		return intent.ascend;
	}
}
