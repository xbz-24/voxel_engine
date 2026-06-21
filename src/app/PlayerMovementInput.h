#pragma once

class Camera;

namespace ve::input
{
	struct InputSnapshot;
}

namespace ve::gameplay
{
	/**
	 * Captures movement keys pressed during a frame.
	 */
	struct PlayerMoveIntent
	{
		bool moveForward = false;
		bool moveBackward = false;
		bool strafeLeft = false;
		bool strafeRight = false;
		bool ascend = false;
		bool descend = false;
	};

	/**
	 * Reads movement keys into an engine-owned intent structure.
	 *
	 * @param input Captured input state.
	 * @return Movement intent for the current frame.
	 */
	PlayerMoveIntent ReadPlayerMoveIntent(const ve::input::InputSnapshot& input) noexcept;

	/**
	 * Applies horizontal movement to the camera from an intent.
	 *
	 * @param intent Movement intent produced by input.
	 * @param camera Camera moved by the player.
	 * @param amount World units to move this frame.
	 */
	void ApplyPlanarMovement(const PlayerMoveIntent& intent, Camera& camera, float amount);

	/**
	 * Applies vertical flying movement to the camera from an intent.
	 *
	 * @param intent Movement intent produced by input.
	 * @param camera Camera moved by the player.
	 * @param amount World units to move this frame.
	 */
	void ApplyFlyingMovement(const PlayerMoveIntent& intent, Camera& camera, float amount);

	/**
	 * Checks whether the intent requests a jump.
	 *
	 * @param intent Movement intent produced by input.
	 * @return True when the player pressed the jump/ascend key.
	 */
	bool WantsJump(const PlayerMoveIntent& intent) noexcept;
}
