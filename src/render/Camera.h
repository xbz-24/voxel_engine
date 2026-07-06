#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
public:
	// TODO: Split camera math from player camera behavior so public apps can plug FPS, orbit, cinematic, or fixed cameras.
	/**
	 * Creates a camera at the default player spawn position.
	 */
	Camera();

	~Camera();

	/**
	 * Returns the camera world position.
	 *
	 * @return Position in world coordinates.
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
	 * @return Normalized right vector on the camera plane.
	 */
	glm::vec3 GetRight() const;

	/**
	 * Returns the up direction derived from yaw and pitch.
	 *
	 * @return Normalized up vector.
	 */
	glm::vec3 GetUp() const;

	/**
	 * Builds the world-to-view transform used by render backends.
	 *
	 * @return Matrix that transforms world coordinates into camera view space.
	 */
	glm::mat4 GetWorldToViewMatrix() const;

	/**
	 * Builds the camera rotation matrix.
	 *
	 * @return Rotation matrix derived from yaw and pitch.
	 */
	glm::mat4 GetRotationMatrix() const;

	/**
	 * Moves the camera along a direction by the provided amount.
	 *
	 * @param direction Movement direction in world space.
	 * @param amount Signed world units to move.
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
	void RebuildOrientationFromYawPitch();

	glm::vec3 _position;
	glm::quat _orientation;
	float _yaw;
	float _pitch;
};
