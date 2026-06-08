#include "Camera.h"

Camera::Camera()
{
	_position = glm::vec3(80.0f, 50.0f, 80.0f);
	_yaw = 0.0f;
	_pitch = 0.0f;
}

Camera::~Camera()
{
}

glm::vec3 Camera::GetPosition() const
{
	return _position;
}

glm::vec3 Camera::GetForward() const
{
	glm::vec4 forward = glm::inverse(GetRotationMatrix()) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	return glm::vec3(forward);
}

glm::vec3 Camera::GetRight() const
{
	glm::vec4 right = glm::inverse(GetRotationMatrix()) * glm::vec4(1.f, 0.f, 0.f, 1.f);
	return glm::vec3(right);
}

glm::vec3 Camera::GetUp() const
{
	glm::vec4 up = glm::inverse(GetRotationMatrix()) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	return glm::vec3(up);
}

glm::mat4 Camera::GetViewMatrix() const
{

	return GetRotationMatrix() * glm::translate(glm::mat4(1.0f), -_position);
}

glm::mat4 Camera::GetRotationMatrix() const
{
	glm::mat4 rotation(1.0f);
	rotation = glm::rotate(rotation, glm::radians(_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	return rotation;
}

void Camera::Move(const glm::vec3& direction, float amount)
{
	_position = _position + direction * amount;
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
	if (_position == position)
	{
		return;
	}
	glm::vec3 direction = glm::normalize(position - _position);
	_pitch = glm::degrees(asinf(-direction.y));
	NormalizePitch();
	_yaw = glm::degrees(atan2f(direction.x, -direction.z));
	NormalizeYaw();
}

void Camera::MoveTo(const glm::vec3& position)
{
	_position = position;
}

void Camera::NormalizeYaw()
{
	_yaw = fmod(_yaw, 360.f);
	if (_yaw < 0.f)
	{
		_yaw += 360.f;
	}
}

void Camera::NormalizePitch()
{
	if (_pitch > 85.f)
	{
		_pitch = 85.f;
	}
	else if (_pitch < -85.f)
	{
		_pitch = -85.f;
	}
}
