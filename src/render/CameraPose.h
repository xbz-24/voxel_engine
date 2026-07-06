#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace ve::rendering
{
	struct CameraPose
	{
		glm::vec3 position{ 80.0f, 50.0f, 80.0f };
		float yaw_degrees = 0.0f;
		float pitch_degrees = 0.0f;
	};

	[[nodiscard]] glm::quat CameraOrientation(const CameraPose& pose);
	[[nodiscard]] glm::vec3 CameraForward(const CameraPose& pose);
	[[nodiscard]] glm::vec3 CameraRight(const CameraPose& pose);
	[[nodiscard]] glm::vec3 CameraUp(const CameraPose& pose);
	[[nodiscard]] glm::mat4 CameraRotationMatrix(const CameraPose& pose);
	[[nodiscard]] glm::mat4 CameraWorldToViewMatrix(const CameraPose& pose);
	[[nodiscard]] CameraPose CameraPoseLookingAt(glm::vec3 position, glm::vec3 target);
	void NormalizeCameraYaw(CameraPose& pose) noexcept;
	void NormalizeCameraPitch(CameraPose& pose) noexcept;
}
