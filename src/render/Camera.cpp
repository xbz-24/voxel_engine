#include "Camera.h"

#include <cmath>

Camera::Camera()
	: _position(80.0f, 50.0f, 80.0f),
	  _orientation(1.0f, 0.0f, 0.0f, 0.0f),
	  _yaw(0.0f),
	  _pitch(0.0f)
{
	RebuildOrientationFromYawPitch();
}

Camera::~Camera() = default;

glm::vec3 Camera::GetPosition() const
{
	return _position;
}

glm::vec3 Camera::GetForward() const
{
	return glm::normalize(_orientation * glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Camera::GetRight() const
{
	return glm::normalize(_orientation * glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Camera::GetUp() const
{
	return glm::normalize(_orientation * glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::GetWorldToViewMatrix() const
{
	return GetRotationMatrix() * glm::translate(glm::mat4(1.0f), -_position);
}

glm::mat4 Camera::GetRotationMatrix() const
{
	return glm::mat4_cast(glm::inverse(_orientation));
}

void Camera::Move(const glm::vec3& direction, float amount)
{
	_position += direction * amount;
}

void Camera::Pitch(float angle)
{
	_pitch += angle;
	NormalizePitch();
	RebuildOrientationFromYawPitch();
}

void Camera::Yaw(float angle)
{
	_yaw += angle;
	NormalizeYaw();
	RebuildOrientationFromYawPitch();
}

void Camera::TurnTo(const glm::vec3& position)
{
	if (_position == position) return;
	const glm::vec3 direction = glm::normalize(position - _position);
	_pitch = glm::degrees(asinf(-direction.y));
	_yaw = glm::degrees(atan2f(direction.x, -direction.z));
	NormalizePitch();
	NormalizeYaw();
	RebuildOrientationFromYawPitch();
}

void Camera::MoveTo(const glm::vec3& position)
{
	_position = position;
}

void Camera::NormalizeYaw()
{
	_yaw = fmod(_yaw, 360.0f);
	if (_yaw < 0.0f) _yaw += 360.0f;
}

void Camera::NormalizePitch()
{
	if (_pitch > 85.0f) _pitch = 85.0f;
	else if (_pitch < -85.0f) _pitch = -85.0f;
}

void Camera::RebuildOrientationFromYawPitch()
{
	glm::mat4 view_rotation(1.0f);
	view_rotation = glm::rotate(view_rotation, glm::radians(_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	view_rotation = glm::rotate(view_rotation, glm::radians(_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	_orientation = glm::normalize(glm::quat_cast(glm::inverse(view_rotation)));
}
