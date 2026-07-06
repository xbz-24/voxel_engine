#include "CameraPose.h"

#include <glm/ext.hpp>

#include <cmath>

namespace ve::rendering
{
	glm::quat CameraOrientation(const CameraPose& pose)
	{
		glm::mat4 view_rotation(1.0f);
		view_rotation = glm::rotate(view_rotation, glm::radians(pose.pitch_degrees), glm::vec3(1.0f, 0.0f, 0.0f));
		view_rotation = glm::rotate(view_rotation, glm::radians(pose.yaw_degrees), glm::vec3(0.0f, 1.0f, 0.0f));
		return glm::normalize(glm::quat_cast(glm::inverse(view_rotation)));
	}

	glm::vec3 CameraForward(const CameraPose& pose)
	{
		return glm::normalize(CameraOrientation(pose) * glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 CameraRight(const CameraPose& pose)
	{
		return glm::normalize(CameraOrientation(pose) * glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 CameraUp(const CameraPose& pose)
	{
		return glm::normalize(CameraOrientation(pose) * glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 CameraRotationMatrix(const CameraPose& pose)
	{
		return glm::mat4_cast(glm::inverse(CameraOrientation(pose)));
	}

	glm::mat4 CameraWorldToViewMatrix(const CameraPose& pose)
	{
		return CameraRotationMatrix(pose) * glm::translate(glm::mat4(1.0f), -pose.position);
	}

	CameraPose CameraPoseLookingAt(glm::vec3 position, glm::vec3 target)
	{
		CameraPose pose{ position, 0.0f, 0.0f };
		if (position == target)
		{
			return pose;
		}
		const glm::vec3 direction = glm::normalize(target - position);
		pose.pitch_degrees = glm::degrees(asinf(-direction.y));
		pose.yaw_degrees = glm::degrees(atan2f(direction.x, -direction.z));
		NormalizeCameraPitch(pose);
		NormalizeCameraYaw(pose);
		return pose;
	}

	void NormalizeCameraYaw(CameraPose& pose) noexcept
	{
		pose.yaw_degrees = fmod(pose.yaw_degrees, 360.0f);
		if (pose.yaw_degrees < 0.0f)
		{
			pose.yaw_degrees += 360.0f;
		}
	}

	void NormalizeCameraPitch(CameraPose& pose) noexcept
	{
		if (pose.pitch_degrees > 85.0f)
		{
			pose.pitch_degrees = 85.0f;
		}
		else if (pose.pitch_degrees < -85.0f)
		{
			pose.pitch_degrees = -85.0f;
		}
	}
}
