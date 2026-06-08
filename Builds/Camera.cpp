#include "Camera.h"

Camera::Camera()
	: _position(80.0f, 50.0f, 80.0f), _yaw(0.0f), _pitch(0.0f)
{
}

Camera::~Camera() = default;

glm::vec3 Camera::GetPosition() const
{
	return _position;
}

glm::vec3 Camera::GetForward() const
{
	return glm::vec3(glm::inverse(GetRotationMatrix()) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
}

glm::vec3 Camera::GetRight() const
{
	return glm::vec3(glm::inverse(GetRotationMatrix()) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

glm::vec3 Camera::GetUp() const
{
	return glm::vec3(glm::inverse(GetRotationMatrix()) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

glm::mat4 Camera::GetViewMatrix() const
{
	return GetRotationMatrix() * glm::translate(glm::mat4(1.0f), -_position);
}

glm::mat4 Camera::GetRotationMatrix() const
{
	glm::mat4 rotation(1.0f);
	rotation = glm::rotate(rotation, glm::radians(_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	return glm::rotate(rotation, glm::radians(_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Move(const glm::vec3& direction, float amount)
{
	_position += direction * amount;
}

void Camera::Pitch(float angle)
{
	_pitch += angle;
	NormalizePitch();
}

void Camera::Yaw(float angle)
{
	_yaw += angle;
	NormalizeYaw();
}

void Camera::TurnTo(const glm::vec3& position)
{
	if (_position == position) return;
	const glm::vec3 direction = glm::normalize(position - _position);
	_pitch = glm::degrees(asinf(-direction.y));
	_yaw = glm::degrees(atan2f(direction.x, -direction.z));
	NormalizePitch();
	NormalizeYaw();
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
