#include "Camera.h"

Camera::Camera()
	: Camera(ve::rendering::CameraPose{})
{
}

Camera::Camera(ve::rendering::CameraPose pose) noexcept
	: pose_(pose)
{
	ve::rendering::NormalizeCameraPitch(pose_);
	ve::rendering::NormalizeCameraYaw(pose_);
}

Camera::~Camera() = default;

glm::vec3 Camera::GetPosition() const
{
	return pose_.position;
}

glm::vec3 Camera::GetForward() const
{
	return ve::rendering::CameraForward(pose_);
}

glm::vec3 Camera::GetRight() const
{
	return ve::rendering::CameraRight(pose_);
}

glm::vec3 Camera::GetUp() const
{
	return ve::rendering::CameraUp(pose_);
}

glm::mat4 Camera::GetWorldToViewMatrix() const
{
	return ve::rendering::CameraWorldToViewMatrix(pose_);
}

glm::mat4 Camera::GetRotationMatrix() const
{
	return ve::rendering::CameraRotationMatrix(pose_);
}

void Camera::Move(const glm::vec3& direction, float amount)
{
	pose_.position += direction * amount;
}

void Camera::Pitch(float angle)
{
	pose_.pitch_degrees += angle;
	NormalizePitch();
}

void Camera::Yaw(float angle)
{
	pose_.yaw_degrees += angle;
	NormalizeYaw();
}

void Camera::TurnTo(const glm::vec3& position)
{
	if (pose_.position == position) return;
	pose_ = ve::rendering::CameraPoseLookingAt(pose_.position, position);
}

void Camera::MoveTo(const glm::vec3& position)
{
	pose_.position = position;
}

void Camera::NormalizeYaw()
{
	ve::rendering::NormalizeCameraYaw(pose_);
}

void Camera::NormalizePitch()
{
	ve::rendering::NormalizeCameraPitch(pose_);
}
