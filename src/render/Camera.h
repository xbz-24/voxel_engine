#pragma once

#include "CameraPose.h"

#include <glm/ext.hpp>
#include <glm/glm.hpp>

class Camera
{
public:
	/** Creates a camera at the default player spawn position. */
	Camera();

	/** @param pose Camera position and yaw/pitch orientation. */
	explicit Camera(ve::rendering::CameraPose pose) noexcept;

	~Camera();

	/** @return Camera position in world coordinates. */
	glm::vec3 GetPosition() const;

	/** @return Normalized forward direction derived from yaw and pitch. */
	glm::vec3 GetForward() const;

	/** @return Normalized right direction on the camera plane. */
	glm::vec3 GetRight() const;

	/** @return Normalized up direction derived from yaw and pitch. */
	glm::vec3 GetUp() const;

	/** @return Matrix that transforms world coordinates into camera view space. */
	glm::mat4 GetWorldToViewMatrix() const;

	/** @return Rotation matrix derived from yaw and pitch. */
	glm::mat4 GetRotationMatrix() const;

	/** Moves along a world-space direction by a signed amount. */
	void Move(const glm::vec3& direction, float amount);

	/** Adds a pitch delta in degrees and clamps it. */
	void Pitch(float angle);

	/** Adds a yaw delta in degrees and normalizes it. */
	void Yaw(float angle);

	/** Rotates the camera to face a world position. */
	void TurnTo(const glm::vec3& position);

	/** Teleports the camera to a world position. */
	void MoveTo(const glm::vec3& position);

	/** Wraps yaw into the 0..360 degree range. */
	void NormalizeYaw();

	/** Clamps pitch so the camera cannot flip vertically. */
	void NormalizePitch();

private:
	ve::rendering::CameraPose pose_;
};
