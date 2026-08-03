	/**
	 * Rotates the camera to face a world position.
	 *
	 * @param position Target world position.
	 */
	void TurnTo(const glm::vec3& position);

	/**
	 * Teleports the camera to a world position.
	 *
	 * @param position New camera world position.
	 */
	void MoveTo(const glm::vec3& position);

	/**
	 * Wraps yaw into the 0..360 degree range.
	 */
	void NormalizeYaw();

	/**
	 * Clamps pitch so the camera cannot flip vertically.
	 */
	void NormalizePitch();

private:
	ve::rendering::CameraPose pose_;
};
