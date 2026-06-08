#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

class Camera
{
public:
	/**
	 * Creates a camera at the default player spawn position.
	 */
	Camera();

	/**
	 * Destroys the camera.
	 */
	~Camera();

	/**
	 * Returns the camera world position.
	 *
	 * @return Current camera position.
	 */
	glm::vec3 GetPosition() const;

	/**
	 * Returns the forward direction derived from yaw and pitch.
	 *
	 * @return Normalized forward vector.
	 */
	glm::vec3 GetForward() const;

	/**
	 * Returns the right direction derived from yaw.
	 *
	 * @return Camera right vector.
	 */
	glm::vec3 GetRight() const;

	/**
	 * Returns the up direction derived from yaw and pitch.
	 *
	 * @return Camera up vector.
	 */
	glm::vec3 GetUp() const;

	/**
	 * Builds the view matrix used by OpenGL.
	 *
	 * @return View matrix that transforms world space into camera space.
	 */
	glm::mat4 GetViewMatrix() const;

	/**
	 * Builds the camera rotation matrix.
	 *
	 * @return Matrix containing yaw and pitch rotations.
	 */
	glm::mat4 GetRotationMatrix() const;

	/**
	 * Moves the camera along a direction.
	 *
	 * @param direction Direction to move in.
	 * @param amount Distance to move.
	 */
	void Move(const glm::vec3& direction, float amount);

	/**
	 * Adds vertical rotation and clamps it.
	 *
	 * @param angle Pitch delta in degrees.
	 */
	void Pitch(float angle);

	/**
	 * Adds horizontal rotation and normalizes it.
	 *
	 * @param angle Yaw delta in degrees.
	 */
	void Yaw(float angle);

	/**
	 * Rotates the camera to face a world position.
	 *
	 * @param position Target world position.
	 */
	void TurnTo(const glm::vec3& position);

	/**
	 * Teleports the camera to a world position.
	 *
	 * @param position New camera position.
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
	glm::vec3 _position;
	float _yaw;
	float _pitch;
};

