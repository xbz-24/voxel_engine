#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

class Camera
{
public:
	/// Creates a camera at the default player spawn position.
	Camera();

	/// Destroys the camera.
	~Camera();

	/// Returns the camera world position.
	glm::vec3 GetPosition() const;

	/// Returns the forward direction derived from yaw and pitch.
	glm::vec3 GetForward() const;

	/// Returns the right direction derived from yaw.
	glm::vec3 GetRight() const;

	/// Returns the up direction derived from yaw and pitch.
	glm::vec3 GetUp() const;

	/// Builds the view matrix used by OpenGL.
	glm::mat4 GetViewMatrix() const;

	/// Builds the camera rotation matrix.
	glm::mat4 GetRotationMatrix() const;

	/// Moves the camera along a direction by the provided amount.
	void Move(const glm::vec3& direction, float amount);

	/// Adds vertical rotation and clamps it.
	void Pitch(float angle);

	/// Adds horizontal rotation and normalizes it.
	void Yaw(float angle);

	/// Rotates the camera to face a world position.
	void TurnTo(const glm::vec3& position);

	/// Teleports the camera to a world position.
	void MoveTo(const glm::vec3& position);

	/// Wraps yaw into the 0..360 degree range.
	void NormalizeYaw();

	/// Clamps pitch so the camera cannot flip vertically.
	void NormalizePitch();

private:
	glm::vec3 _position;
	float _yaw;
	float _pitch;
};
